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

    std::unique_ptr<dsp::eq::EQToolParams> eqParams;
    std::unique_ptr<dsp::waveshaper::Params> waveshaperParams;
    std::unique_ptr<dsp::signal_gen::Params> signalGenParams;
    std::unique_ptr<dsp::analog_eq::Params> analogEQParams;
    std::unique_ptr<dsp::band_splitter::Params> bandSplitParams;
    std::unique_ptr<dsp::brickwall::Params> brickwallParams;
    std::unique_ptr<dsp::svf::Params> svfParams;
};

struct PluginExtraState : chowdsp::NonParamState
{
    PluginExtraState();
    ~PluginExtraState();

    std::unique_ptr<dsp::waveshaper::ExtraState> waveshaperExtraState;
};
} // namespace state

using State = chowdsp::PluginStateImpl<state::PluginParams, state::PluginExtraState>;
