#pragma once

#include "dsp/Waveshaper/WaveshaperProcessor.h"
#include "state/PluginState.h"

namespace gui::waveshaper
{
class WaveshaperPlot : public juce::Component
{
public:
    WaveshaperPlot (State& pluginState, dsp::waveshaper::Params& wsParams);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    dsp::waveshaper::WaveshaperProcessor processor;
    chowdsp::WaveshaperPlot plotter;

    chowdsp::ScopedCallbackList callbacks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperPlot)
};
}
