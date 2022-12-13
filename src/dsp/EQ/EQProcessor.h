#pragma once

#include <pch.h>

namespace dsp::eq
{
struct Params
{
    static constexpr size_t numBands = 8;
    using EQParams = chowdsp::EQ::StandardEQParameters<numBands>;

    // @TODO: brickwall filters
    static constexpr std::string_view bandTypeChoices =
        "HPF 6 dB/Oct,"
        "HPF 12 dB/Oct,"
        "HPF 18 dB/Oct,"
        "HPF 24 dB/Oct,"
        "HPF 48 dB/Oct,"
        "Low-Shelf,"
        "Bell,"
        "Notch,"
        "High-Shelf,"
        "LPF 6 dB/Oct,"
        "LPF 12 dB/Oct,"
        "LPF 18 dB/Oct,"
        "LPF 24 dB/Oct,"
        "LPF 48 dB/Oct";

    EQParams eqParams {
        "eq_params",
        EQParams::EQParameterHandles {
            EQParams::EQBandParams { 0, "eq_band_0", "EQ Band ", ParameterVersionHints::version1_0_0, 60.0f, bandTypeChoices, 1, 0.5f },
            EQParams::EQBandParams { 1, "eq_band_1", "EQ Band ", ParameterVersionHints::version1_0_0, 125.0f, bandTypeChoices, 6, 0.5f },
            EQParams::EQBandParams { 2, "eq_band_2", "EQ Band ", ParameterVersionHints::version1_0_0, 250.0f, bandTypeChoices, 6, 0.5f },
            EQParams::EQBandParams { 3, "eq_band_3", "EQ Band ", ParameterVersionHints::version1_0_0, 500.0f, bandTypeChoices, 6, 0.5f },
            EQParams::EQBandParams { 4, "eq_band_4", "EQ Band ", ParameterVersionHints::version1_0_0, 1000.0f, bandTypeChoices, 6, 0.5f },
            EQParams::EQBandParams { 5, "eq_band_5", "EQ Band ", ParameterVersionHints::version1_0_0, 2000.0f, bandTypeChoices, 6, 0.5f },
            EQParams::EQBandParams { 6, "eq_band_6", "EQ Band ", ParameterVersionHints::version1_0_0, 4000.0f, bandTypeChoices, 6, 0.5f },
            EQParams::EQBandParams { 7, "eq_band_7", "EQ Band ", ParameterVersionHints::version1_0_0, 8000.0f, bandTypeChoices, 10, 0.5f },
        }
    };

    chowdsp::BoolParameter::Ptr linearPhaseMode { juce::ParameterID { "eq_linear_phase", ParameterVersionHints::version1_0_0 },
                                                  "Linear Phase On/Off",
                                                  false };
};

class EQProcessor
{
public:
    EQProcessor() = default;

    template <typename PluginParams>
    void initialise (PluginParams& pluginParams)
    {
        params = &pluginParams.eqParams;
    }

    void prepare (const juce::dsp::ProcessSpec& spec);
    void processBlock (const chowdsp::BufferView<float>& buffer);

private:
    Params* params = nullptr;

    auto getEQParams()
    {
        return Params::EQParams::getEQParameters (params->eqParams.eqParams);
    }

    static constexpr auto DecrampedMode = chowdsp::CoefficientCalculators::CoefficientCalculationMode::Decramped;
    using EQFloat = xsimd::batch<double>;
    using EQBand = chowdsp::EQ::EQBand<EQFloat,
                                       chowdsp::FirstOrderHPF<EQFloat>,
                                       chowdsp::SecondOrderHPF<EQFloat, DecrampedMode>,
                                       chowdsp::ButterworthFilter<3, chowdsp::ButterworthFilterType::Highpass, EQFloat>,
                                       chowdsp::ButterworthFilter<4, chowdsp::ButterworthFilterType::Highpass, EQFloat>,
                                       chowdsp::ButterworthFilter<8, chowdsp::ButterworthFilterType::Highpass, EQFloat>,
                                       chowdsp::LowShelfFilter<EQFloat, DecrampedMode>,
                                       chowdsp::PeakingFilter<EQFloat, DecrampedMode>,
                                       chowdsp::NotchFilter<EQFloat, DecrampedMode>,
                                       chowdsp::HighShelfFilter<EQFloat, DecrampedMode>,
                                       chowdsp::SecondOrderBPF<EQFloat, DecrampedMode>,
                                       chowdsp::FirstOrderLPF<EQFloat>,
                                       chowdsp::SecondOrderLPF<EQFloat, DecrampedMode>,
                                       chowdsp::ButterworthFilter<3, chowdsp::ButterworthFilterType::Lowpass, EQFloat>,
                                       chowdsp::ButterworthFilter<4, chowdsp::ButterworthFilterType::Lowpass, EQFloat>,
                                       chowdsp::ButterworthFilter<8, chowdsp::ButterworthFilterType::Lowpass, EQFloat>>;
    chowdsp::EQ::EQProcessor<EQFloat, Params::EQParams::EQNumBands, EQBand> eq;

    chowdsp::Buffer<EQFloat> eqBuffer;

    struct PrototypeEQ
    {
        using Params = Params::EQParams::Params;

        using EQBand = chowdsp::EQ::EQBand<float,
                                           chowdsp::FirstOrderHPF<float>,
                                           chowdsp::SecondOrderHPF<float, DecrampedMode>,
                                           chowdsp::ButterworthFilter<3, chowdsp::ButterworthFilterType::Highpass, float>,
                                           chowdsp::ButterworthFilter<4, chowdsp::ButterworthFilterType::Highpass, float>,
                                           chowdsp::ButterworthFilter<8, chowdsp::ButterworthFilterType::Highpass, float>,
                                           chowdsp::LowShelfFilter<float, DecrampedMode>,
                                           chowdsp::PeakingFilter<float, DecrampedMode>,
                                           chowdsp::NotchFilter<float, DecrampedMode>,
                                           chowdsp::HighShelfFilter<float, DecrampedMode>,
                                           chowdsp::SecondOrderBPF<float, DecrampedMode>,
                                           chowdsp::FirstOrderLPF<float>,
                                           chowdsp::SecondOrderLPF<float, DecrampedMode>,
                                           chowdsp::ButterworthFilter<3, chowdsp::ButterworthFilterType::Lowpass, float>,
                                           chowdsp::ButterworthFilter<4, chowdsp::ButterworthFilterType::Lowpass, float>,
                                           chowdsp::ButterworthFilter<8, chowdsp::ButterworthFilterType::Lowpass, float>>;
        chowdsp::EQ::EQProcessor<float, dsp::eq::Params::EQParams::EQNumBands, EQBand> eq;

        PrototypeEQ() = default;

        void setParameters (const Params& eqParams)
        {
            Params::setEQParameters (eq, eqParams);
        }

        void prepare (const juce::dsp::ProcessSpec& spec)
        {
            eq.prepare (spec);
        }

        void reset()
        {
            eq.reset();
        }

        void processBlock (juce::AudioBuffer<float>& buffer)
        {
            eq.processBlock (buffer);
        }
    } protoEQ;

    chowdsp::EQ::LinearPhaseEQ<PrototypeEQ> linPhaseEQ;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQProcessor)
};
} // namespace dsp::eq
