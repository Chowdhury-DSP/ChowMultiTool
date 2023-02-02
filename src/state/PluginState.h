#pragma once

#include "dsp/BandSplitter/BandSplitterProcessor.h"
#include "dsp/Brickwall/BrickwallProcessor.h"
#include "dsp/EQ/EQProcessor.h"
#include "dsp/PultecEQ/PultecEQProcessor.h"
#include "dsp/SignalGenerator/SignalGeneratorProcessor.h"
#include "dsp/Waveshaper/WaveshaperProcessor.h"
#include "dsp/SVF/SVFProcessor.h"

namespace state
{
namespace Hints = ParameterVersionHints;

struct PluginParams : chowdsp::ParamHolder
{
    PluginParams()
    {
        add (toolParam,
             eqParams,
             waveshaperParams,
             signalGenParams,
             pultecEQParams,
             bandSplitParams,
             brickwallParams,
             svfParams);
    }

    static const juce::StringArray toolChoices;
    chowdsp::ChoiceParameter::Ptr toolParam {
        juce::ParameterID { "tool", Hints::version1_0_0 },
        "Tool",
        toolChoices,
        0
    };

    dsp::eq::EQToolParams eqParams;
    dsp::waveshaper::Params waveshaperParams;
    dsp::signal_gen::Params signalGenParams;
    dsp::pultec::Params pultecEQParams;
    dsp::band_splitter::Params bandSplitParams;
    dsp::brickwall::Params brickwallParams;
    dsp::svf::Params svfParams;
};
} // namespace state

using State = chowdsp::PluginStateImpl<state::PluginParams>;
