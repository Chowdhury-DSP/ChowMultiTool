#pragma once

#include "dsp/Shared/EQHelpers.h"
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

enum class KeytrackMonoMode
{
    Highest_Note_Priority = 1,
    Lowest_Note_Priority = 2,
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
             keytrackOffset,
             keytrackMonoMode,
             qParam,
             mode,
             type,
             plainType,
             arpType,
             arpLimitMode,
             wernerType,
             wernerDamping,
             wernerDrive);
    }

    chowdsp::FreqHzParameter::Ptr cutoff {
        juce::ParameterID { "svf_cutoff", ParameterVersionHints::version1_0_0 },
        "SVF Cutoff",
        chowdsp::ParamUtils::createNormalisableRange (20.0f, 20'000.0f, 2000.0f),
        1000.0f
    };

    chowdsp::BoolParameter::Ptr keytrack {
        juce::ParameterID { "svf_keytrack", ParameterVersionHints::version1_0_0 },
        "SVF Keytrack",
        false
    };

    chowdsp::FloatParameter::Ptr keytrackOffset {
        juce::ParameterID { "svf_keytrack_offset", ParameterVersionHints::version1_0_0 },
        "SVF Keytrack Offset",
        juce::NormalisableRange { -36.0f, 36.0f },
        0.0f,
        [] (float val) -> juce::String
        {
            auto baseString = chowdsp::ParamUtils::floatValToString (val);
            if (val > 0.0f)
                baseString = "+" + baseString;
            return baseString + " st";
        },
        &chowdsp::ParamUtils::stringToFloatVal
    };

    chowdsp::EnumChoiceParameter<KeytrackMonoMode>::Ptr keytrackMonoMode {
        juce::ParameterID { "svf_keytrack_mono_mode", ParameterVersionHints::version1_0_0 },
        "SVF Keytrack Mono Mode",
        KeytrackMonoMode::Highest_Note_Priority
    };

    chowdsp::FloatParameter::Ptr qParam {
        juce::ParameterID { "svf_q_value", ParameterVersionHints::version1_0_0 },
        "SVF Q",
        chowdsp::ParamUtils::createNormalisableRange (0.5f, 30.0f, 5.0f),
        5.0f,
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

    chowdsp::PercentParameter::Ptr wernerDrive {
        juce::ParameterID { "svf_werner_drive", ParameterVersionHints::version1_0_0 },
        "SVF Werner Drive",
        0.0f,
    };
};

class SVFProcessor
{
public:
    explicit SVFProcessor (const Params& svfParams) : params (svfParams) {}

    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();
    void processBlock (const chowdsp::BufferView<float>& buffer, const juce::MidiBuffer& midi) noexcept;
    const auto& getEQHelper() const { return eqHelper; }
    const auto& getHelper() { return eqHelper; }

    static float midiNoteToHz (float midiNote);

private:
    void processSmallBlock (const chowdsp::BufferView<float>& buffer) noexcept;
    void processKeytracking (const juce::MidiBuffer& midi) noexcept;
    int getLowestNotePriority() const noexcept;
    int getHighestNotePriority() const noexcept;

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

    chowdsp::Gain<float> driveInGain, driveOutGain;
    EQHelpers eqHelper;

    static constexpr size_t maxPolyphony = 32;
    std::array<int, maxPolyphony> playingNotes {};
    int currentPlayingNote = 69;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SVFProcessor)
};
} // namespace dsp::svf
