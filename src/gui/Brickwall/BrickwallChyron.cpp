#include "BrickwallChyron.h"
#include "gui/Shared/Colours.h"

namespace gui::brickwall
{
BrickwallChyron::BrickwallChyron (chowdsp::PluginState& pluginState,
                                  dsp::brickwall::Params& params,
                                  const chowdsp::HostContextProvider& hcp)
    : state (pluginState),
      cutoffSlider (state, params.cutoff.get(), &hcp)
{
    cutoffSlider.setName ("Cutoff");
    addAndMakeVisible (cutoffSlider);
}

void BrickwallChyron::resized()
{
    auto bounds = getLocalBounds();
    cutoffSlider.setBounds (bounds.reduced (proportionOfHeight (0.2f)));
}

void BrickwallChyron::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds();

    g.setColour (juce::Colours::black.withAlpha (0.75f));
    g.fillRoundedRectangle (bounds.toFloat(), 2.5f);

    g.setColour (colours::linesColour);
    g.drawRoundedRectangle (bounds.toFloat(), 2.5f, 1.0f);
}
} // namespace gui::brickwall
