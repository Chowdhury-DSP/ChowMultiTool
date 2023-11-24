#pragma once

#include "gui/Shared/SpectrumAnalyserTask.h"
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

    std::atomic<bool> isOpen;
};

class EQProcessor
{
public:
    explicit EQProcessor (const EQToolParams& eqParams) : params (eqParams),
                                                          preSpectrumAnalyserTask(std::make_unique<gui::SpectrumAnalyserTask>()),
                                                          postSpectrumAnalyserTask(std::make_unique<gui::SpectrumAnalyserTask>()){}

    void prepare (const juce::dsp::ProcessSpec& spec);
    void processBlock (const chowdsp::BufferView<float>& buffer);

    int getLatencySamples() const;
    std::pair<gui::SpectrumAnalyserTask&, gui::SpectrumAnalyserTask&> getSpectrumAnalyserTasks() { return {*preSpectrumAnalyserTask, *postSpectrumAnalyserTask}; }

private:
    const EQToolParams& params;

    auto getEQParams()
    {
        return EQToolParams::EQParams::getEQParameters (params.eqParams.eqParams);
    }

    static constexpr auto DecrampedMode = chowdsp::CoefficientCalculators::CoefficientCalculationMode::Decramped;
    template <typename FloatType>
    using EQBand = chowdsp::EQ::EQBand<FloatType,
                                       chowdsp::FirstOrderHPF<FloatType>,
                                       chowdsp::SecondOrderHPF<FloatType, DecrampedMode>,
                                       chowdsp::ButterworthFilter<3, chowdsp::ButterworthFilterType::Highpass, FloatType>,
                                       chowdsp::ButterworthFilter<4, chowdsp::ButterworthFilterType::Highpass, FloatType>,
                                       chowdsp::ButterworthFilter<8, chowdsp::ButterworthFilterType::Highpass, FloatType>,
                                       chowdsp::LowShelfFilter<FloatType, DecrampedMode>,
                                       chowdsp::PeakingFilter<FloatType, DecrampedMode>,
                                       chowdsp::NotchFilter<FloatType, DecrampedMode>,
                                       chowdsp::HighShelfFilter<FloatType, DecrampedMode>,
                                       chowdsp::FirstOrderLPF<FloatType>,
                                       chowdsp::SecondOrderLPF<FloatType, DecrampedMode>,
                                       chowdsp::ButterworthFilter<3, chowdsp::ButterworthFilterType::Lowpass, FloatType>,
                                       chowdsp::ButterworthFilter<4, chowdsp::ButterworthFilterType::Lowpass, FloatType>,
                                       chowdsp::ButterworthFilter<8, chowdsp::ButterworthFilterType::Lowpass, FloatType>>;
    using EQFloat = xsimd::batch<double>;
    chowdsp::EQ::EQProcessor<EQFloat, EQToolParams::EQParams::EQNumBands, EQBand<EQFloat>> eq;

    chowdsp::Buffer<double> doubleBuffer;
    chowdsp::Buffer<EQFloat> eqBuffer;

    using LinearPhaseProtoEQ = chowdsp::EQ::LinearPhasePrototypeEQ<double, EQToolParams::EQParams::Params, EQToolParams::EQParams::EQNumBands, EQBand<double>>;
    chowdsp::EQ::LinearPhaseEQ<LinearPhaseProtoEQ> linPhaseEQ;
    std::unique_ptr<gui::SpectrumAnalyserTask> preSpectrumAnalyserTask;
    std::unique_ptr<gui::SpectrumAnalyserTask> postSpectrumAnalyserTask;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQProcessor)
};
} // namespace dsp::eq
