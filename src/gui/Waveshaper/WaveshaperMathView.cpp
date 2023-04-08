#include "WaveshaperMathView.h"
#include "gui/Shared/Colours.h"
#include "gui/Shared/ErrorMessageView.h"

namespace gui::waveshaper
{
using namespace spline;

WaveshaperMathView::WaveshaperMathView (dsp::waveshaper::ExtraState& wsExtraState, juce::UndoManager& undoManager)
    : mathState (wsExtraState.mathState),
      um (undoManager)
{
    prevPoints = mathState.get();

    mathBox.setMultiLine (false);
    mathBox.setJustification (juce::Justification::centred);
    mathBox.setTextToShowWhenEmpty ("tanh (x)", colours::linesColour.withAlpha (0.4f));
    mathBox.setColour (juce::TextEditor::backgroundColourId, colours::backgroundDark);
    mathBox.setColour (juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
    mathBox.setColour (juce::TextEditor::textColourId, colours::linesColour);
    mathBox.setColour (juce::TextEditor::highlightColourId, colours::plotColour.withAlpha (0.4f));
    mathBox.setColour (juce::CaretComponent::caretColourId, colours::plotColour);
    addAndMakeVisible (mathBox);
    mathBox.onReturnKey = [this]
    { attemptToLoadNewMathShaper(); };

    enterButton.setColour (juce::TextButton::buttonColourId, colours::boxColour.withAlpha (0.75f));
    enterButton.setColour (juce::TextButton::textColourOnId, colours::linesColour);
    enterButton.setColour (juce::TextButton::textColourOffId, colours::linesColour);
    enterButton.setColour (juce::ComboBox::outlineColourId, colours::linesColour);
    addAndMakeVisible (enterButton);
    enterButton.onClick = [this]
    { attemptToLoadNewMathShaper(); };
}

void WaveshaperMathView::attemptToLoadNewMathShaper()
{
    if (mathBox.isEmpty())
        return;

    static constexpr auto scaler = float (numDrawPoints - 1) / (splineBounds.xMax - splineBounds.xMin);
    static constexpr auto offset = -splineBounds.xMin * scaler;

    float expressionX = 0.0f;
    exprtk::symbol_table<float> symbol_table;
    symbol_table.add_variable ("x", expressionX);
    symbol_table.add_constants();

    exprtk::expression<float> expression;
    expression.register_symbol_table (symbol_table);

    exprtk::parser<float> parser;
    parser.compile (mathBox.getText().toStdString(), expression);

    SplinePoints points;
    for (auto [index, point] : chowdsp::enumerate (points))
    {
        expressionX = ((float) index - offset) / scaler;
        const auto result = expression.value();

        if (std::isnan (result) || std::abs (result) > 20.0f)
        {
            ErrorMessageView::showErrorMessage ("Invalid Math Function!",
                                                "The math function you have entered is invalid!\n\n"
                                                "Make sure that your function:\n"
                                                "- Has no typos\n"
                                                "- Is in terms of only one variable \"x\"\n"
                                                "- Returns a valid result for inputs in the range [-4, 4]",
                                                "OK",
                                                this);
            return;
        }

        point.x = expressionX;
        point.y = result;
    }

    mathState.set (points);

    if (auto* parent = getParentComponent())
        parent->repaint();

    um.beginNewTransaction ("Waveshaper Math");
    um.perform (new UndoableSplineSet { mathState, prevPoints, getParentComponent() });

    prevPoints = points;
}

void WaveshaperMathView::visibilityChanged()
{
    mathBox.clear();
}

void WaveshaperMathView::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::white.withAlpha (0.3f));
}

void WaveshaperMathView::resized()
{
    const auto centre = getLocalBounds().getCentre();
    mathBox.setBounds (juce::Rectangle { proportionOfWidth (0.8f), proportionOfHeight (0.1f) }
                           .withCentre (centre));
    mathBox.setFont ((float) mathBox.getHeight() * 0.75f);

    enterButton.setBounds (juce::Rectangle { proportionOfWidth (0.25f), proportionOfHeight (0.1f) }
                               .withCentre (centre)
                               .withY (mathBox.getBottom() + proportionOfHeight (0.1f)));
}

juce::Path WaveshaperMathView::getDrawnPath (std::optional<chowdsp::WaveshaperPlotParams>&& params) const
{
    const auto splineInfo = createSpline (mathState.get());

    if (! params.has_value())
        params.emplace (splineBounds);

    const auto floatWidth = (float) getWidth();
    const auto floatHeight = (float) getHeight();

    juce::Path splinePath;
    splinePath.startNewSubPath ({ splineBounds.xMin, (float) evaluateSpline (splineInfo, (double) splineBounds.xMin) });
    const auto xInc = double ((splineBounds.xMax - splineBounds.xMin) / floatWidth);
    for (double x = (double) splineBounds.xMin + xInc; x <= (double) splineBounds.xMax; x += xInc)
    {
        splinePath.lineTo (juce::Point { x, evaluateSpline (splineInfo, x) }.toFloat());
    }

    splinePath.applyTransform (juce::AffineTransform::fromTargetPoints (juce::Point { params->xMin, params->yMin },
                                                                        juce::Point { 0.0f, floatHeight },
                                                                        juce::Point { params->xMin, params->yMax },
                                                                        juce::Point { 0.0f, 0.0f },
                                                                        juce::Point { params->xMax, params->yMax },
                                                                        juce::Point { floatWidth, 0.0f }));
    return splinePath;
}
} // namespace gui::waveshaper
