#pragma once

#include <pch.h>

namespace dsp::eq
{
struct Params
{
    chowdsp::BoolParameter::Ptr linearPhaseMode { juce::ParameterID { "eq_linear_phase", ParameterVersionHints::version1_0_0 },
                                                  "Linear Phase On/Off",
                                                  false };
};

class EQProcessor
{
public:
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
