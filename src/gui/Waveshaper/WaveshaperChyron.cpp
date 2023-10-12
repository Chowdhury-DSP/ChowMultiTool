#include "WaveshaperChyron.h"
#include "gui/Shared/Colours.h"

namespace gui::waveshaper
{
WaveshaperChyron::WaveshaperChyron (chowdsp::PluginState& pluginState,
                                    dsp::waveshaper::Params& params,
                                    const chowdsp::HostContextProvider& hcp)
    : state (pluginState),
      gainSlider (state, params.gainParam.get(), &hcp)
{
    gainSlider.setName ("Gain");
    addAndMakeVisible (gainSlider);
}

void WaveshaperChyron::resized()
{
    auto bounds = getLocalBounds();
    gainSlider.setBounds (bounds.reduced (proportionOfHeight (0.2f)));
}

void WaveshaperChyron::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds();

    g.setColour (juce::Colours::black.withAlpha (0.75f));
    g.fillRoundedRectangle (bounds.toFloat(), 2.5f);

    g.setColour (colours::linesColour);
    g.drawRoundedRectangle (bounds.toFloat(), 2.5f, 1.0f);
}
} // namespace gui::waveshaper
