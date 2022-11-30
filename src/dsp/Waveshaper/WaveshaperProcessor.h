#pragma once

#include <pch.h>

namespace dsp::waveshaper
{
struct Params
{
    static constexpr std::string_view name = "waveshaper_params";
};

class WaveshaperProcessor
{
public:
    static constexpr std::string_view name = "Waveshaper";

    WaveshaperProcessor() = default;

    template <typename PluginParams>
    void initialise (PluginParams& pluginParams) { params = &pluginParams.waveshaperParams; }

    void prepare (const juce::dsp::ProcessSpec& spec);
    void processBlock (const chowdsp::BufferView<float>& buffer);

private:
    Params* params = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperProcessor)
};
}
