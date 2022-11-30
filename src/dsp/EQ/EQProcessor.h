#pragma once

#include <pch.h>

namespace dsp::eq
{
struct Params
{
    static constexpr std::string_view name = "eq_params";
    chowdsp::BoolParameter::Ptr linearPhaseMode { juce::ParameterID { "eq_linear_phase", ParameterVersionHints::version1_0_0 },
                                                  "Linear Phase On/Off",
                                                  false };
};

class EQProcessor
{
public:
    static constexpr std::string_view name = "EQ";

    EQProcessor() = default;

    template <typename PluginParams>
    void initialise (PluginParams& pluginParams) { params = &pluginParams.eqParams; }

    void prepare (const juce::dsp::ProcessSpec& spec);
    void processBlock (const chowdsp::BufferView<float>& buffer);

private:
    Params* params = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQProcessor)
};
} // namespace dsp::eq
