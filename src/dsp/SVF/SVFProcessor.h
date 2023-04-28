#pragma once

#include <pch.h>

namespace dsp::svf
{
enum class SVFType
{
    Plain = 1,
    ARP = 2,
    Werner = 4,
};

enum class PlainType
{
    Lowpass = 1,
    Highpass = 2,
    Bandpass = 4,
    MultiMode = 8,
};
} // namespace dsp::svf

template <>
constexpr magic_enum::customize::customize_t
    magic_enum::customize::enum_name<chowdsp::WernerFilterType> (chowdsp::WernerFilterType value) noexcept
{
    using chowdsp::WernerFilterType;
    switch (value)
    {
        case WernerFilterType::Lowpass2:
            return "2nd-Order LPF";
        case WernerFilterType::Bandpass2:
            return "2nd-Order BPF";
        case WernerFilterType::Highpass2:
            return "2nd-Order HPF";
        case WernerFilterType::Lowpass4:
            return "4th-Order LPF";
        case WernerFilterType::MultiMode:
            return "Multi-Mode";
    }
    return default_tag;
}

template <>
constexpr magic_enum::customize::customize_t
    magic_enum::customize::enum_name<dsp::svf::PlainType> (dsp::svf::PlainType value) noexcept
{
    using dsp::svf::PlainType;
    if (value == PlainType::MultiMode)
        return "Multi-Mode";
    return default_tag;
}

namespace dsp::svf
{
struct Params : public chowdsp::ParamHolder
{
    Params()
    {
        add (cutoff,
             keytrack,
             qParam,
             mode,
             type,
             plainType,
             arpType,
             arpLimitMode,
             wernerType,
             wernerDamping);
    }

    chowdsp::BoolParameter::Ptr keytrack {
        juce::ParameterID { "svf_keytrack", ParameterVersionHints::version1_0_0 },
        "SVF Keytrack",
        false
    };

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
    explicit SVFProcessor (const Params& svfParams) : params (svfParams) {}

    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();
    void processBlock (const chowdsp::BufferView<float>& buffer, const juce::MidiBuffer& midi) noexcept;

private:
    void processSmallBlock (const chowdsp::BufferView<float>& buffer) noexcept;

    const Params& params;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> cutoffSmooth;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> qSmooth;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> modeSmooth;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> dampingSmooth;

    std::tuple<
        chowdsp::SVFLowpass<>,
        chowdsp::SVFHighpass<>,
        chowdsp::SVFBandpass<>,
        chowdsp::SVFMultiMode<>>
        plainFilters;

    chowdsp::ARPFilter<float> arpFilter;
    chowdsp::WernerFilter wernerFilter;

    static constexpr size_t maxPolyphony = 32;
    std::array<int, maxPolyphony> playingNotes {};
    int currentPlayingNote = 69;
    int getLowestNotePriority() const noexcept;
    int getHighestNotePriority() const noexcept;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SVFProcessor)
};
} // namespace dsp::svf
