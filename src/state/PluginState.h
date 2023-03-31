#pragma once

#include <pch.h>

namespace dsp
{
namespace eq
{
    struct EQToolParams;
}
namespace waveshaper
{
    struct Params;
    struct ExtraState;
} // namespace waveshaper
namespace signal_gen
{
    struct Params;
}
namespace analog_eq
{
    struct Params;
}
namespace band_splitter
{
    struct Params;
}
namespace brickwall
{
    struct Params;
}
namespace svf
{
    struct Params;
}
} // namespace dsp

namespace state
{
namespace Hints = ParameterVersionHints;

struct PluginParams : chowdsp::ParamHolder
{
    PluginParams();
    ~PluginParams();

    static const juce::StringArray toolChoices;
    chowdsp::ChoiceParameter::Ptr toolParam {
        juce::ParameterID { "tool", Hints::version1_0_0 },
        "Tool",
        toolChoices,
        0
    };

    chowdsp::LocalPointer<dsp::eq::EQToolParams, 5488> eqParams;
    chowdsp::LocalPointer<dsp::waveshaper::Params, 224> waveshaperParams;
    chowdsp::LocalPointer<dsp::signal_gen::Params, 192> signalGenParams;
    chowdsp::LocalPointer<dsp::analog_eq::Params, 272> analogEQParams;
    chowdsp::LocalPointer<dsp::band_splitter::Params, 208> bandSplitParams;
    chowdsp::LocalPointer<dsp::brickwall::Params, 208> brickwallParams;
    chowdsp::LocalPointer<dsp::svf::Params, 304> svfParams;
};

struct PluginExtraState : chowdsp::NonParamState
{
    PluginExtraState();
    ~PluginExtraState();

    std::unique_ptr<dsp::waveshaper::ExtraState> waveshaperExtraState;
};
} // namespace state

using State = chowdsp::PluginStateImpl<state::PluginParams, state::PluginExtraState>;
