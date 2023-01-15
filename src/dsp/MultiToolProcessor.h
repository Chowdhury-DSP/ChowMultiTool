#pragma once

#include "state/PluginState.h"

namespace dsp
{
using ToolTypes = types_list::TypesList<
    eq::EQProcessor,
    waveshaper::WaveshaperProcessor,
    signal_gen::SignalGeneratorProcessor,
    pultec::PultecEQProcessor,
    band_splitter::BandSplitterProcessor>;

class MultiToolProcessor
{
public:
    MultiToolProcessor (juce::AudioProcessor& plugin, state::PluginParams& pluginParams);

    void prepare (const juce::dsp::ProcessSpec& spec);
    void processBlock (juce::AudioBuffer<float>& buffer);

private:
    juce::AudioProcessor& plugin;
    state::PluginParams& params;

    ToolTypes::Types tools;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiToolProcessor)
};
} // namespace dsp
