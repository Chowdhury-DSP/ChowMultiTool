#pragma once

#include "dsp/Waveshaper/WaveshaperProcessor.h"
#include "gui/Shared/Fonts.h"
#include "gui/Shared/TextSlider.h"
#include "state/PluginState.h"

namespace gui::waveshaper
{
class WaveshaperChyron : public juce::Component
{
public:
    WaveshaperChyron (chowdsp::PluginState& pluginState,
                      dsp::waveshaper::Params& params,
                      const chowdsp::HostContextProvider& hcp);

    void resized() override;
    void paint (juce::Graphics& g) override;

private:
    chowdsp::PluginState& state;

    TextSlider gainSlider;

    SharedFonts fonts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperChyron)
};
} // namespace gui::waveshaper
