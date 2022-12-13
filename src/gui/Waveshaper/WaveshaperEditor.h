#pragma once

#include "dsp/Waveshaper/WaveshaperProcessor.h"
#include "state/PluginState.h"

namespace gui::waveshaper
{
class WaveshaperEditor : public juce::Component
{
public:
    WaveshaperEditor (State& pluginState, dsp::waveshaper::Params& wsParams);

    void resized() override;

private:
    chowdsp::ParametersView<State, dsp::waveshaper::Params> paramsView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperEditor)
};
} // namespace gui::waveshaper
