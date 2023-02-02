#pragma once

#include <pch.h>

namespace dsp::svf
{
enum class SVFType
{
    Plain,
    ARP,
    Werner,
};

enum class PlainType
{
    Lowpass,
    Highpass,
    Bandpass,
    Allpass,
    Notch,
    Bell,
    LowShelf,
    HighShelf,
    Multi,
};

struct Params : public chowdsp::ParamHolder
{
    Params()
    {
        add (cutoff,
             qParam,
             gain,
             mode,
             type,
             plainType,
             arpType,
             arpLimitMode,
             wernerType,
             wernerDamping);
    }

    chowdsp::FreqHzParameter::Ptr cutoff {
        juce::ParameterID { "svf_cutoff", ParameterVersionHints::version1_0_0 },
        "SVF Cutoff",
        chowdsp::ParamUtils::createNormalisableRange (20.0f, 20'000.0f, 2000.0f),
        1000.0f
    };

    chowdsp::FloatParameter::Ptr qParam {
        juce::ParameterID { "svf_q_value", ParameterVersionHints::version1_0_0 },
        "SVF Q",
        chowdsp::ParamUtils::createNormalisableRange (0.5f, 20.0f, 2.5f),
        chowdsp::CoefficientCalculators::butterworthQ<float>,
        &chowdsp::ParamUtils::floatValToString,
        &chowdsp::ParamUtils::stringToFloatVal
    };

    chowdsp::GainDBParameter::Ptr gain {
        juce::ParameterID { "svf_gain", ParameterVersionHints::version1_0_0 },
        "SVF Gain",
        juce::NormalisableRange { -18.0f, 18.0f },
        0.0f,
    };

    chowdsp::PercentParameter::Ptr mode {
        juce::ParameterID { "svf_mode", ParameterVersionHints::version1_0_0 },
        "SVF Mode",
        0.0f,
        true
    };

    chowdsp::EnumChoiceParameter<SVFType>::Ptr type {
        juce::ParameterID { "svf_type", ParameterVersionHints::version1_0_0 },
        "SVF Type",
        SVFType::Plain
    };

    chowdsp::EnumChoiceParameter<PlainType>::Ptr plainType {
        juce::ParameterID { "svf_plain_type", ParameterVersionHints::version1_0_0 },
        "SVF Plain Type",
        PlainType::Lowpass
    };

    chowdsp::EnumChoiceParameter<chowdsp::ARPFilterType>::Ptr arpType {
        juce::ParameterID { "svf_arp_type", ParameterVersionHints::version1_0_0 },
        "SVF ARP Type",
        chowdsp::ARPFilterType::Lowpass
    };

    chowdsp::BoolParameter::Ptr arpLimitMode {
        juce::ParameterID { "svf_arp_limit_mode", ParameterVersionHints::version1_0_0 },
        "SVF ARP Limit Mode",
        false
    };

    chowdsp::EnumChoiceParameter<chowdsp::WernerFilterType>::Ptr wernerType {
        juce::ParameterID { "svf_werner_type", ParameterVersionHints::version1_0_0 },
        "SVF Werner Type",
        chowdsp::WernerFilterType::Lowpass4
    };

    chowdsp::PercentParameter::Ptr wernerDamping {
        juce::ParameterID { "svf_werner_damp", ParameterVersionHints::version1_0_0 },
        "SVF Werner Damping",
        0.5f,
    };
};

class SVFProcessor
{
public:
    SVFProcessor() = default;

    template <typename PluginParams>
    void initialise (PluginParams& pluginParams)
    {
        params = &pluginParams.svfParams;
    }

    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();
    void processBlock (const chowdsp::BufferView<float>& buffer);

private:
    Params* params = nullptr;

    std::tuple<
        chowdsp::SVFLowpass<>,
        chowdsp::SVFHighpass<>,
        chowdsp::SVFBandpass<>,
        chowdsp::SVFAllpass<>,
        chowdsp::SVFNotch<>,
        chowdsp::SVFBell<>,
        chowdsp::SVFHighShelf<>,
        chowdsp::SVFLowShelf<>,
        chowdsp::SVFMultiMode<>
        > plainFilters;

    chowdsp::ARPFilter<float> arpFilter;
    chowdsp::WernerFilter wernerFilter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SVFProcessor)
};
} // namespace dsp::svf
