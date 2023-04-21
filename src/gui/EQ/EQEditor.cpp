#include "EQEditor.h"
#include "BottomBar.h"
#include "gui/Shared/Colours.h"

namespace gui::eq
{
EQEditor::EQEditor (State& pluginState, dsp::eq::EQToolParams& eqParams, const chowdsp::HostContextProvider& hcp)
    : plot (pluginState, eqParams.eqParams, hcp),
      paramsView (pluginState, eqParams),
      linearPhaseButton ("Vector/arrow-right-arrow-left-solid.svg", colours::thumbColours[0], colours::linesColour),
      linearPhaseAttach (eqParams.linearPhaseMode, pluginState, linearPhaseButton),
      drawButton ("Vector/pencil-solid.svg", colours::thumbColours[0], colours::linesColour)
{
    bottomBar = std::make_unique<BottomBar> (pluginState, eqParams);

    addAndMakeVisible (plot);
    addAndMakeVisible (bottomBar.get());
    addAndMakeVisible (linearPhaseButton);

    linearPhaseButton.setTooltip ("Linear Phase");

    addAndMakeVisible (drawButton);
    drawButton.onStateChange = [this]
    { plot.toggleDrawView (drawButton.getToggleState()); };

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
}
} // namespace gui::eq
