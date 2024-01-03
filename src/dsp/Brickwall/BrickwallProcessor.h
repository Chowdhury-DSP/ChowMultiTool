#pragma once

#include "gui/Shared/SpectrumAnalyserTask.h"
#include <pch.h>

namespace dsp::brickwall
{
enum class FilterMode
{
    LPF = 1,
    HPF = 2,
};

enum class Order
{
    m2nd_Order = 1,
    m4th_Order = 2,
    m6th_Order = 4,
    m8th_Order = 8,
    m10th_Order = 16,
    m12th_Order = 32,
    m14th_Order = 64,
    m16th_Order = 128,
};

enum class FilterType
{
    Butterworth = 1,
    ChebyshevII = 2,
    Elliptic = 4,
};

struct ExtraState
{
    std::atomic<bool> isEditorOpen { false };
    chowdsp::StateValue<std::atomic_bool, bool> showPostSpectrum { "brickwall_show_post_spectrum", true };
};

} // namespace dsp::brickwall

template <>
constexpr magic_enum::customize::customize_t
    magic_enum::customize::enum_name<dsp::brickwall::Order> (dsp::brickwall::Order value) noexcept
{
    using dsp::brickwall::Order;
    switch (value)
    {
        case Order::m2nd_Order:
            return "2nd Order";
        case Order::m4th_Order:
            return "4th Order";
        case Order::m6th_Order:
            return "6th Order";
        case Order::m8th_Order:
            return "8th Order";
        case Order::m10th_Order:
            return "10th Order";
        case Order::m12th_Order:
            return "12th Order";
        case Order::m14th_Order:
            return "14th Order";
        case Order::m16th_Order:
            return "16th Order";
    }
    return default_tag;
}

namespace dsp::brickwall
{
struct Params : chowdsp::ParamHolder
{
    Params()
    {
        add (filterMode,
             order,
             filterType,
             cutoff);
    }

    chowdsp::EnumChoiceParameter<FilterMode>::Ptr filterMode {
        juce::ParameterID { "brickwall_mode", ParameterVersionHints::version1_0_0 },
        "Brickwall Filter Mode",
        FilterMode::LPF
    };

    chowdsp::EnumChoiceParameter<Order>::Ptr order {
        juce::ParameterID { "brickwall_order", ParameterVersionHints::version1_0_0 },
        "Brickwall Order",
        Order::m12th_Order
    };

    chowdsp::EnumChoiceParameter<FilterType>::Ptr filterType {
        juce::ParameterID { "brickwall_type", ParameterVersionHints::version1_0_0 },
        "Brickwall Filter Type",
        FilterType::Butterworth
    };

    chowdsp::FreqHzParameter::Ptr cutoff {
        juce::ParameterID { "brickwall_cutoff", ParameterVersionHints::version1_0_0 },
        "Brickwall Cutoff",
        chowdsp::ParamUtils::createNormalisableRange (20.0f, 20'000.0f, 2000.0f),
        1000.0f
    };
};

class BrickwallProcessor
{
public:
    explicit BrickwallProcessor (const Params& brickwallParams, const dsp::brickwall::ExtraState& es) : params (brickwallParams), extraState (es)
    {
        postSpectrumAnalyserTask.SpectrumAnalyserUITask.setDBRange(-60, 5);
    }

    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();
    void processBlock (const chowdsp::BufferView<float>& buffer) noexcept;


    std::pair<gui::SpectrumAnalyserTask::Optional, gui::SpectrumAnalyserTask::Optional> getSpectrumAnalyserTasks()
    {
        return { std::nullopt, std::ref (postSpectrumAnalyserTask.SpectrumAnalyserUITask) };
    }
    //    gui::SpectrumAnalyserTask::SpectrumAnalyserBackgroundTask& getSpectrumAnalyserTasks() { return postSpectrumAnalyserTask.SpectrumAnalyserUITask; }

private:
    int getFilterTypeIndex() const;

    const Params& params;
    const ExtraState& extraState;

    using EQBand = chowdsp::EQ::EQBand<
        float,
        chowdsp::ButterworthFilter<2, chowdsp::ButterworthFilterType::Lowpass>,
        chowdsp::ButterworthFilter<2, chowdsp::ButterworthFilterType::Highpass>,
        chowdsp::ButterworthFilter<4, chowdsp::ButterworthFilterType::Lowpass>,
        chowdsp::ButterworthFilter<4, chowdsp::ButterworthFilterType::Highpass>,
        chowdsp::ButterworthFilter<6, chowdsp::ButterworthFilterType::Lowpass>,
        chowdsp::ButterworthFilter<6, chowdsp::ButterworthFilterType::Highpass>,
        chowdsp::ButterworthFilter<8, chowdsp::ButterworthFilterType::Lowpass>,
        chowdsp::ButterworthFilter<8, chowdsp::ButterworthFilterType::Highpass>,
        chowdsp::ButterworthFilter<10, chowdsp::ButterworthFilterType::Lowpass>,
        chowdsp::ButterworthFilter<10, chowdsp::ButterworthFilterType::Highpass>,
        chowdsp::ButterworthFilter<12, chowdsp::ButterworthFilterType::Lowpass>,
        chowdsp::ButterworthFilter<12, chowdsp::ButterworthFilterType::Highpass>,
        chowdsp::ButterworthFilter<14, chowdsp::ButterworthFilterType::Lowpass>,
        chowdsp::ButterworthFilter<14, chowdsp::ButterworthFilterType::Highpass>,
        chowdsp::ButterworthFilter<16, chowdsp::ButterworthFilterType::Lowpass>,
        chowdsp::ButterworthFilter<16, chowdsp::ButterworthFilterType::Highpass>,
        chowdsp::ChebyshevIIFilter<2, chowdsp::ChebyshevFilterType::Lowpass>,
        chowdsp::ChebyshevIIFilter<2, chowdsp::ChebyshevFilterType::Highpass>,
        chowdsp::ChebyshevIIFilter<4, chowdsp::ChebyshevFilterType::Lowpass>,
        chowdsp::ChebyshevIIFilter<4, chowdsp::ChebyshevFilterType::Highpass>,
        chowdsp::ChebyshevIIFilter<6, chowdsp::ChebyshevFilterType::Lowpass>,
        chowdsp::ChebyshevIIFilter<6, chowdsp::ChebyshevFilterType::Highpass>,
        chowdsp::ChebyshevIIFilter<8, chowdsp::ChebyshevFilterType::Lowpass>,
        chowdsp::ChebyshevIIFilter<8, chowdsp::ChebyshevFilterType::Highpass>,
        chowdsp::ChebyshevIIFilter<10, chowdsp::ChebyshevFilterType::Lowpass>,
        chowdsp::ChebyshevIIFilter<10, chowdsp::ChebyshevFilterType::Highpass>,
        chowdsp::ChebyshevIIFilter<12, chowdsp::ChebyshevFilterType::Lowpass>,
        chowdsp::ChebyshevIIFilter<12, chowdsp::ChebyshevFilterType::Highpass>,
        chowdsp::ChebyshevIIFilter<14, chowdsp::ChebyshevFilterType::Lowpass>,
        chowdsp::ChebyshevIIFilter<14, chowdsp::ChebyshevFilterType::Highpass>,
        chowdsp::ChebyshevIIFilter<16, chowdsp::ChebyshevFilterType::Lowpass>,
        chowdsp::ChebyshevIIFilter<16, chowdsp::ChebyshevFilterType::Highpass>,
        chowdsp::EllipticFilter<2, chowdsp::EllipticFilterType::Lowpass>,
        chowdsp::EllipticFilter<2, chowdsp::EllipticFilterType::Highpass>,
        chowdsp::EllipticFilter<4, chowdsp::EllipticFilterType::Lowpass>,
        chowdsp::EllipticFilter<4, chowdsp::EllipticFilterType::Highpass>,
        chowdsp::EllipticFilter<6, chowdsp::EllipticFilterType::Lowpass>,
        chowdsp::EllipticFilter<6, chowdsp::EllipticFilterType::Highpass>,
        chowdsp::EllipticFilter<8, chowdsp::EllipticFilterType::Lowpass>,
        chowdsp::EllipticFilter<8, chowdsp::EllipticFilterType::Highpass>,
        chowdsp::EllipticFilter<10, chowdsp::EllipticFilterType::Lowpass>,
        chowdsp::EllipticFilter<10, chowdsp::EllipticFilterType::Highpass>,
        chowdsp::EllipticFilter<12, chowdsp::EllipticFilterType::Lowpass>,
        chowdsp::EllipticFilter<12, chowdsp::EllipticFilterType::Highpass>,
        chowdsp::EllipticFilter<14, chowdsp::EllipticFilterType::Lowpass>,
        chowdsp::EllipticFilter<14, chowdsp::EllipticFilterType::Highpass>,
        chowdsp::EllipticFilter<16, chowdsp::EllipticFilterType::Lowpass>,
        chowdsp::EllipticFilter<16, chowdsp::EllipticFilterType::Highpass>>;
    EQBand filter;

    gui::SpectrumAnalyserTask postSpectrumAnalyserTask;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BrickwallProcessor)
};
} // namespace dsp::brickwall
