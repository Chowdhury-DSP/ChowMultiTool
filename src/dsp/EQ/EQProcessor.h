#pragma once

#include <pch.h>

namespace dsp::eq
{
struct EQToolParams : chowdsp::ParamHolder
{
    static constexpr size_t numBands = 8;
    using EQParams = chowdsp::EQ::StandardEQParameters<numBands>;

    EQToolParams()
    {
        add (eqParams, linearPhaseMode);
    }

    // @TODO: brickwall filters
    inline static const juce::StringArray bandTypeChoices {
        "HPF 6 dB/Oct",
        "HPF 12 dB/Oct",
        "HPF 18 dB/Oct",
        "HPF 24 dB/Oct",
        "HPF 48 dB/Oct",
        "Low-Shelf",
        "Bell",
        "Notch",
        "High-Shelf",
        "LPF 6 dB/Oct",
        "LPF 12 dB/Oct",
        "LPF 18 dB/Oct",
        "LPF 24 dB/Oct",
        "LPF 48 dB/Oct",
    };

    EQParams eqParams {
        {
            EQParams::EQBandParams { .bandIndex = 0,
                                     .bandParamPrefix = "eq_band_0",
                                     .bandNamePrefix = "EQ Band ",
                                     .versionHint = ParameterVersionHints::version1_0_0,
                                     .bandTypeChoices = bandTypeChoices,
                                     .defaultEQBandTypeChoice = 1,
                                     .freqDefault = 60.0f,
                                     .qRange = chowdsp::ParamUtils::createNormalisableRange (0.5f, 20.0f, chowdsp::CoefficientCalculators::butterworthQ<float>) },
            EQParams::EQBandParams { .bandIndex = 1,
                                     .bandParamPrefix = "eq_band_1",
                                     .bandNamePrefix = "EQ Band ",
                                     .versionHint = ParameterVersionHints::version1_0_0,
                                     .bandTypeChoices = bandTypeChoices,
                                     .defaultEQBandTypeChoice = 6,
                                     .freqDefault = 125.0f,
                                     .qRange = chowdsp::ParamUtils::createNormalisableRange (0.5f, 20.0f, chowdsp::CoefficientCalculators::butterworthQ<float>) },
            EQParams::EQBandParams { .bandIndex = 2,
                                     .bandParamPrefix = "eq_band_2",
                                     .bandNamePrefix = "EQ Band ",
                                     .versionHint = ParameterVersionHints::version1_0_0,
                                     .bandTypeChoices = bandTypeChoices,
                                     .defaultEQBandTypeChoice = 6,
                                     .freqDefault = 250.0f,
                                     .qRange = chowdsp::ParamUtils::createNormalisableRange (0.5f, 20.0f, chowdsp::CoefficientCalculators::butterworthQ<float>) },
            EQParams::EQBandParams { .bandIndex = 3,
                                     .bandParamPrefix = "eq_band_3",
                                     .bandNamePrefix = "EQ Band ",
                                     .versionHint = ParameterVersionHints::version1_0_0,
                                     .bandTypeChoices = bandTypeChoices,
                                     .defaultEQBandTypeChoice = 6,
                                     .freqDefault = 500.0f,
                                     .qRange = chowdsp::ParamUtils::createNormalisableRange (0.5f, 20.0f, chowdsp::CoefficientCalculators::butterworthQ<float>) },
            EQParams::EQBandParams { .bandIndex = 4,
                                     .bandParamPrefix = "eq_band_4",
                                     .bandNamePrefix = "EQ Band ",
                                     .versionHint = ParameterVersionHints::version1_0_0,
                                     .bandTypeChoices = bandTypeChoices,
                                     .defaultEQBandTypeChoice = 6,
                                     .freqDefault = 1000.0f,
                                     .qRange = chowdsp::ParamUtils::createNormalisableRange (0.5f, 20.0f, chowdsp::CoefficientCalculators::butterworthQ<float>) },
            EQParams::EQBandParams { .bandIndex = 5,
                                     .bandParamPrefix = "eq_band_5",
                                     .bandNamePrefix = "EQ Band ",
                                     .versionHint = ParameterVersionHints::version1_0_0,
                                     .bandTypeChoices = bandTypeChoices,
                                     .defaultEQBandTypeChoice = 6,
                                     .freqDefault = 2000.0f,
                                     .qRange = chowdsp::ParamUtils::createNormalisableRange (0.5f, 20.0f, chowdsp::CoefficientCalculators::butterworthQ<float>) },
            EQParams::EQBandParams { .bandIndex = 6,
                                     .bandParamPrefix = "eq_band_6",
                                     .bandNamePrefix = "EQ Band ",
                                     .versionHint = ParameterVersionHints::version1_0_0,
                                     .bandTypeChoices = bandTypeChoices,
                                     .defaultEQBandTypeChoice = 6,
                                     .freqDefault = 4000.0f,
                                     .qRange = chowdsp::ParamUtils::createNormalisableRange (0.5f, 20.0f, chowdsp::CoefficientCalculators::butterworthQ<float>) },
            EQParams::EQBandParams { .bandIndex = 7,
                                     .bandParamPrefix = "eq_band_7",
                                     .bandNamePrefix = "EQ Band ",
                                     .versionHint = ParameterVersionHints::version1_0_0,
                                     .bandTypeChoices = bandTypeChoices,
                                     .defaultEQBandTypeChoice = 10,
                                     .freqDefault = 8000.0f,
                                     .qRange = chowdsp::ParamUtils::createNormalisableRange (0.5f, 20.0f, chowdsp::CoefficientCalculators::butterworthQ<float>) },
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
    EQToolParams* params = nullptr;

    auto getEQParams()
    {
        return EQToolParams::EQParams::getEQParameters (params->eqParams.eqParams);
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
    chowdsp::EQ::EQProcessor<EQFloat, EQToolParams::EQParams::EQNumBands, EQBand> eq;

    chowdsp::Buffer<EQFloat> eqBuffer;

    struct PrototypeEQ
    {
        using Params = EQToolParams::EQParams::Params;

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
        chowdsp::EQ::EQProcessor<float, dsp::eq::EQToolParams::EQParams::EQNumBands, EQBand> eq;

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
