#pragma once

#include "dsp/EQ/EQProcessor.h"
#include "dsp/SignalGenerator/SignalGeneratorProcessor.h"
#include "dsp/Waveshaper/WaveshaperProcessor.h"
#include "dsp/PultecEQ/PultecEQProcessor.h"

namespace state
{
namespace Hints = ParameterVersionHints;

struct PluginParams
{
    static const juce::StringArray toolChoices;
    chowdsp::ChoiceParameter::Ptr toolParam {
        juce::ParameterID { "tool", Hints::version1_0_0 },
        "Tool",
        toolChoices,
        0
    };

    dsp::eq::Params eqParams;
    dsp::waveshaper::Params waveshaperParams;
    dsp::signal_gen::Params signalGenParams;
    dsp::pultec::Params pultecEQParams;
};
} // namespace state

using State = chowdsp::PluginState<state::PluginParams>;
