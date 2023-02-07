#pragma once

#include <pch.h>

namespace dsp::brickwall
{
enum class FilterMode
{
    LPF,
    HPF,
};

enum class Order
{
    m2nd_Order,
    m4th_Order,
    m6th_Order,
    m8th_Order,
    m10th_Order,
    m12th_Order,
    m14th_Order,
    m16th_Order,
};

enum class FilterType
{
    Butterworth,
    ChebyshevII,
    Elliptic,
};

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
        Order::m12th_Order,
        std::initializer_list<std::pair<char, char>> { { 'm', 24 }, { '_', ' ' } }
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
    explicit BrickwallProcessor (Params& brickwallParams) : params (brickwallParams) {}

    void prepare (const juce::dsp::ProcessSpec& spec);
    void processBlock (const chowdsp::BufferView<float>& buffer) noexcept;

private:
    int getFilterTypeIndex() const;

    const Params& params;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BrickwallProcessor)
};
} // namespace dsp::brickwall
