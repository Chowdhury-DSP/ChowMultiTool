#include "SVFChyron.h"
#include "gui/Shared/Colours.h"

namespace gui::svf
{
SVFChyron::SVFChyron (chowdsp::PluginState& pluginState, dsp::svf::Params& params, const chowdsp::HostContextProvider& hcp)
    : freqSlider (pluginState, params.cutoff.get(), &hcp),
      keytrackOffsetSlider (pluginState, params.keytrackOffset.get(), &hcp)
{
    freqSlider.setName ("Freq");
    addAndMakeVisible (freqSlider);

    keytrackOffsetSlider.setName ("Offset");
    addAndMakeVisible (keytrackOffsetSlider);
}

void SVFChyron::keytrackParamChanged (bool keytrackModeOn)
{
    freqSlider.setVisible (! keytrackModeOn);
    keytrackOffsetSlider.setVisible (keytrackModeOn);
}

void SVFChyron::resized()
{
    freqSlider.setBounds (juce::Rectangle { 0, 0, getWidth(), getHeight() }.reduced (proportionOfWidth (0.05f), proportionOfHeight (0.05f)));
    keytrackOffsetSlider.setBounds (freqSlider.getBounds());
}

void SVFChyron::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds();

    g.setColour (juce::Colours::black.withAlpha (0.75f));
    g.fillRoundedRectangle (bounds.toFloat(), 2.5f);

    g.setColour (colours::linesColour);
    g.drawRoundedRectangle (bounds.toFloat(), 2.5f, 1.0f);
}
} // namespace gui::svf
