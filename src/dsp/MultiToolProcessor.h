#pragma once

#include "state/PluginState.h"

namespace dsp
{
using ToolTypes = chowdsp::TypesList<
    eq::EQProcessor,
    waveshaper::WaveshaperProcessor,
    signal_gen::SignalGeneratorProcessor,
    pultec::PultecEQProcessor
    >;

class MultiToolProcessor
{
public:
    explicit MultiToolProcessor (state::PluginParams& pluginParams);

    void prepare (const juce::dsp::ProcessSpec& spec);
    void processBlock (const chowdsp::BufferView<float>& buffer);

private:
    state::PluginParams& params;

    ToolTypes::Types tools;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiToolProcessor)
};
} // namespace dsp
