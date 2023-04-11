#include "WaveshaperEditor.h"
#include "BottomBar.h"
#include "gui/Shared/Colours.h"

namespace gui::waveshaper
{
WaveshaperEditor::WaveshaperEditor (State& pluginState, dsp::waveshaper::Params& wsParams)
    : params (wsParams),
      plot (pluginState, wsParams),
      foldFuzzControls (pluginState, wsParams),
      freeDrawButton ("Vector/pencil-solid.svg", colours::plotColour, colours::linesColour),
      mathButton ("Vector/calculator-solid.svg", colours::plotColour, colours::linesColour),
      pointsButton ("Vector/eye-dropper-solid.svg", colours::plotColour, colours::linesColour)
{
    bottomBar = std::make_unique<BottomBar> (pluginState, wsParams);

    addAndMakeVisible (plot);
    addAndMakeVisible (bottomBar.get());

    addChildComponent (foldFuzzControls);
    foldFuzzControls.setVisible (params.shapeParam->get() == dsp::waveshaper::Shapes::Fold_Fuzz);

    addChildComponent (freeDrawButton);
    freeDrawButton.setVisible (wsParams.shapeParam->get() == dsp::waveshaper::Shapes::Free_Draw);
    freeDrawButton.onStateChange = [this]
    { plot.toggleDrawMode (freeDrawButton.getToggleState()); };

    addChildComponent (mathButton);
    mathButton.setVisible (wsParams.shapeParam->get() == dsp::waveshaper::Shapes::Math);
    mathButton.onStateChange = [this]
    { plot.toggleMathMode (mathButton.getToggleState()); };

    addChildComponent (pointsButton);
    pointsButton.setVisible (wsParams.shapeParam->get() == dsp::waveshaper::Shapes::Spline);
    pointsButton.onStateChange = [this]
    { plot.togglePointsMode (pointsButton.getToggleState()); };

    callbacks += {
        pluginState.addParameterListener (
            *wsParams.shapeParam,
            chowdsp::ParameterListenerThread::MessageThread,
            [this]
            {
                foldFuzzControls.setVisible (params.shapeParam->get() == dsp::waveshaper::Shapes::Fold_Fuzz);
                resized();
            }),
        pluginState.addParameterListener (
            *wsParams.shapeParam,
            chowdsp::ParameterListenerThread::MessageThread,
            [this, &wsParams]
            {
                const auto isFreeDrawMode = wsParams.shapeParam->get() == dsp::waveshaper::Shapes::Free_Draw;
                freeDrawButton.setVisible (isFreeDrawMode);
                if (! isFreeDrawMode)
                    freeDrawButton.setToggleState (false, juce::sendNotification);

                const auto isMathMode = wsParams.shapeParam->get() == dsp::waveshaper::Shapes::Math;
                mathButton.setVisible (isMathMode);
                if (! isMathMode)
                    mathButton.setToggleState (false, juce::sendNotification);

                const auto isPointsMode = wsParams.shapeParam->get() == dsp::waveshaper::Shapes::Spline;
                pointsButton.setVisible (isPointsMode);
                if (! isPointsMode)
                    pointsButton.setToggleState (false, juce::sendNotification);
            }),
    };
}

void WaveshaperEditor::resized()
{
    auto bounds = getLocalBounds();
    bottomBar->setBounds (bounds.removeFromBottom (proportionOfHeight (0.075f)));

    if (params.shapeParam->get() == dsp::waveshaper::Shapes::Fold_Fuzz)
        foldFuzzControls.setBounds (bounds.removeFromRight (proportionOfWidth (0.15f)));

    plot.setBounds (bounds);

    const auto pad = proportionOfWidth (0.005f);
    const auto dim = proportionOfWidth (0.05f);
    const auto editButtonBounds = juce::Rectangle { bounds.getWidth() - pad - dim, pad, dim, dim };
    freeDrawButton.setBounds (editButtonBounds);
    mathButton.setBounds (editButtonBounds);
    pointsButton.setBounds (editButtonBounds);
}

void WaveshaperEditor::paint (juce::Graphics& g)
{
    g.setGradientFill (juce::ColourGradient { colours::backgroundLight,
                                              juce::Point { 0.0f, 0.0f },
                                              colours::backgroundDark,
                                              juce::Point { (float) getWidth() * 0.35f, (float) getHeight() * 0.5f },
                                              false });
    g.fillAll();
}
} // namespace gui::waveshaper
