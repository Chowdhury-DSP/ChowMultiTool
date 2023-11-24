#include "EQEditor.h"
#include "BottomBar.h"
#include "gui/Shared/Colours.h"

namespace gui::eq
{
EQEditor::EQEditor (State& pluginState, dsp::eq::EQToolParams& eqParams, const chowdsp::HostContextProvider& hcp, std::pair<SpectrumAnalyserTask&, SpectrumAnalyserTask&> spectrumAnalyserTasks)
    : params (eqParams),
      plot (pluginState, eqParams.eqParams, hcp),
      paramsView (pluginState, eqParams),
      linearPhaseButton ("Vector/arrow-right-arrow-left-solid.svg", colours::thumbColours[0], colours::linesColour),
      linearPhaseAttach (eqParams.linearPhaseMode, pluginState, linearPhaseButton),
      spectrumAnalyser (std::make_unique<SpectrumAnalyser>(plot, spectrumAnalyserTasks)),
      drawButton ("Vector/pencil-solid.svg", colours::linesColour, colours::linesColour),
      drawCheckButton ("Vector/square-check-regular.svg", colours::linesColour, colours::linesColour),
      drawXButton ("Vector/rectangle-xmark-regular.svg", colours::linesColour, colours::linesColour)
{
    params.isOpen.store (true);

    bottomBar = std::make_unique<BottomBar> (pluginState, eqParams);

    addAndMakeVisible (plot);
    addAndMakeVisible (bottomBar.get());
    addAndMakeVisible (linearPhaseButton);
    addAndMakeVisible (*spectrumAnalyser);

    linearPhaseButton.setTooltip ("Linear Phase");

    addAndMakeVisible (drawButton);
    drawButton.onClick = [this]
    {
        linearPhaseButton.setVisible (false);
        drawButton.setVisible (false);
        drawCheckButton.setVisible (true);
        drawXButton.setVisible (true);
        plot.toggleDrawView (true, false);
    };
    addChildComponent (drawCheckButton);
    drawCheckButton.onClick = [this]
    {
        linearPhaseButton.setVisible (true);
        drawButton.setVisible (true);
        drawCheckButton.setVisible (false);
        drawXButton.setVisible (false);
        plot.toggleDrawView (true, true);
    };
    addChildComponent (drawXButton);
    drawXButton.onClick = [this]
    {
        linearPhaseButton.setVisible (true);
        drawButton.setVisible (true);
        drawCheckButton.setVisible (false);
        drawXButton.setVisible (false);
        plot.toggleDrawView (false, false);
    };
}

EQEditor::~EQEditor()
{
    params.isOpen.store (false);
}

void EQEditor::paint (juce::Graphics& g)
{
    g.setGradientFill (juce::ColourGradient { colours::backgroundLight,
                                              juce::Point { 0.0f, 0.0f },
                                              colours::backgroundDark,
                                              juce::Point { (float) getWidth() * 0.35f, (float) getHeight() * 0.5f },
                                              false });
    g.fillAll();
}

void EQEditor::resized()
{
    auto bounds = getLocalBounds();

    plot.setBounds (bounds);
    bottomBar->setBounds (bounds.removeFromBottom (proportionOfHeight (0.075f)));

    const auto pad = proportionOfWidth (0.005f);
    const auto buttonDim = proportionOfWidth (0.035f);
    linearPhaseButton.setBounds (bounds.getWidth() - pad - buttonDim, pad, buttonDim, buttonDim);

    drawButton.setBounds (linearPhaseButton.getBoundsInParent().translated (-pad - buttonDim, 0));
    drawCheckButton.setBounds (drawButton.getBoundsInParent());
    drawXButton.setBounds (linearPhaseButton.getBoundsInParent());
    spectrumAnalyser->setBounds (getLocalBounds());
    spectrumAnalyser->toBack();
}
} // namespace gui::eq
