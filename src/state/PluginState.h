#pragma once

#include <pch.h>

namespace dsp
{
namespace eq
{
    struct EQToolParams;
    struct ExtraState;
} // namespace eq
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
    struct ExtraState;
} // namespace analog_eq
namespace band_splitter
{
    struct Params;
    struct ExtraState;
} // namespace band_splitter
namespace brickwall
{
    struct Params;
    struct ExtraState;
} // namespace brickwall
namespace svf
{
    struct Params;
    struct ExtraState;
} // namespace svf
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

    chowdsp::BoolParameter::Ptr bypassParam {
        juce::ParameterID { "internal_bypass", Hints::version1_1_0 },
        "Bypass",
        false
    };

    std::unique_ptr<dsp::eq::EQToolParams> eqParams;
    chowdsp::LocalPointer<dsp::waveshaper::Params, 384, 16> waveshaperParams;
    chowdsp::LocalPointer<dsp::signal_gen::Params, 384, 16> signalGenParams;
    chowdsp::LocalPointer<dsp::analog_eq::Params, 512, 16> analogEQParams;
    chowdsp::LocalPointer<dsp::band_splitter::Params, 384, 16> bandSplitParams;
    chowdsp::LocalPointer<dsp::brickwall::Params, 384, 16> brickwallParams;
    chowdsp::LocalPointer<dsp::svf::Params, 512, 16> svfParams;
};

struct PluginExtraState : chowdsp::NonParamState
{
    PluginExtraState();
    ~PluginExtraState();

    std::unique_ptr<dsp::waveshaper::ExtraState> waveshaperExtraState;
    chowdsp::LocalPointer<dsp::analog_eq::ExtraState, 128> analogEqExtraState;
    chowdsp::LocalPointer<dsp::eq::ExtraState, 128> eqExtraState;
    chowdsp::LocalPointer<dsp::brickwall::ExtraState, 128> brickwallExtraState;
    chowdsp::LocalPointer<dsp::svf::ExtraState, 128> svfExtraState;
    chowdsp::LocalPointer<dsp::band_splitter::ExtraState, 128> bandSplitterExtraState;
    chowdsp::StateValue<juce::Point<int>> editorBounds { "editor_bounds", { 750, 500 } };
};
} // namespace state

using State = chowdsp::PluginStateImpl<state::PluginParams, state::PluginExtraState>;
