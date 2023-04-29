#pragma once

#include "state/PluginState.h"

#include "dsp/AnalogEQ/AnalogEQProcessor.h"
#include "dsp/BandSplitter/BandSplitterProcessor.h"
#include "dsp/Brickwall/BrickwallProcessor.h"
#include "dsp/EQ/EQProcessor.h"
#include "dsp/SVF/SVFProcessor.h"
#include "dsp/SignalGenerator/SignalGeneratorProcessor.h"
#include "dsp/Waveshaper/WaveshaperProcessor.h"

namespace dsp
{
using ToolTypes = types_list::TypesList<
    eq::EQProcessor,
    waveshaper::WaveshaperProcessor,
    signal_gen::SignalGeneratorProcessor,
    analog_eq::AnalogEQProcessor,
    band_splitter::BandSplitterProcessor,
    brickwall::BrickwallProcessor,
    svf::SVFProcessor>;

class MultiToolProcessor
{
public:
    MultiToolProcessor (juce::AudioProcessor& plugin, State& pluginState);

    void prepare (const juce::dsp::ProcessSpec& spec);
    void processBlock (juce::AudioBuffer<float>& buffer, const juce::MidiBuffer& midiBuffer);

private:
    void recalculateLatency();

    juce::AudioProcessor& plugin;
    state::PluginParams& params;
    chowdsp::ScopedCallbackList latencyChangeCallbacks;

    ToolTypes::Types tools;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiToolProcessor)
};
} // namespace dsp
