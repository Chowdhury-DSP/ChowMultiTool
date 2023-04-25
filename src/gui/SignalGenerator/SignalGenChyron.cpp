#include "SignalGenChyron.h"
#include "gui/Shared/Colours.h"

namespace gui::signal_gen
{
SignalGenChyron::SignalGenChyron (chowdsp::PluginState& pluginState, dsp::signal_gen::Params& params, const chowdsp::HostContextProvider& hcp)
    : freqSlider (pluginState, params.frequency.get(), &hcp),
      gainSlider (pluginState, params.gain.get(), &hcp)
{
    freqSlider.setName ("Freq");
    addAndMakeVisible (freqSlider);
    gainSlider.setName ("Gain");
    addAndMakeVisible (gainSlider);
}

void SignalGenChyron::resized()
{
    const auto bounds = getLocalBounds();

    const auto halfHeight = bounds.proportionOfHeight (0.5f);
    freqSlider.setBounds (juce::Rectangle { 0, 0, getWidth(), halfHeight }.reduced (proportionOfWidth (0.05f), proportionOfHeight (0.05f)));
    gainSlider.setBounds (juce::Rectangle { 0, halfHeight, getWidth(), halfHeight }.reduced (proportionOfWidth (0.05f), proportionOfHeight (0.05f)));
}

void SignalGenChyron::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds();

    g.setColour (juce::Colours::black.withAlpha (0.75f));
    g.fillRoundedRectangle (bounds.toFloat(), 2.5f);

    g.setColour (colours::linesColour);
    g.drawRoundedRectangle (bounds.toFloat(), 2.5f, 1.0f);

}
} // namespace gui::signal_gen
