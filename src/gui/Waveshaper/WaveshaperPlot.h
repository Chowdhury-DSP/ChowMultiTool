#pragma once

#include "dsp/Waveshaper/WaveshaperProcessor.h"
#include "state/PluginState.h"

namespace gui::waveshaper
{
class WaveshaperPlot : public juce::Slider
{
public:
    WaveshaperPlot (State& pluginState, dsp::waveshaper::Params& wsParams);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    chowdsp::WaveshaperPlot plotter;
    chowdsp::ScopedCallbackList callbacks;
    chowdsp::SliderAttachment gainAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperPlot)
};
} // namespace gui::waveshaper
