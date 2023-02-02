
#pragma once

#include <pch.h>

namespace dsp::signal_gen
{
enum class Oscillator
{
    Sine,
    Saw,
    Square,
    Triangle,
};

struct Params : chowdsp::ParamHolder
{
    Params()
    {
        add (oscillatorChoice, frequency, gain);
    }

    chowdsp::EnumChoiceParameter<Oscillator>::Ptr oscillatorChoice {
        juce::ParameterID { "signal_gen_oscillator", ParameterVersionHints::version1_0_0 },
        "Signal Generator Oscillator",
        Oscillator::Sine
    };

    chowdsp::FreqHzParameter::Ptr frequency {
        juce::ParameterID { "signal_gen_frequency", ParameterVersionHints::version1_0_0 },
        "Signal Generator Frequency",
        chowdsp::ParamUtils::createNormalisableRange (10.0f, 25000.0f, 1000.0f),
        1000.0f
    };

    chowdsp::GainDBParameter::Ptr gain {
        juce::ParameterID { "signal_gen_gain", ParameterVersionHints::version1_0_0 },
        "Signal Generator Gain",
        juce::NormalisableRange { -60.0f, 0.0f },
        -18.0f
    };
};

class SignalGeneratorProcessor
{
public:
    SignalGeneratorProcessor() = default;

    template <typename PluginParams>
    void initialise (PluginParams& pluginParams)
    {
        params = &pluginParams.signalGenParams;
    }

    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();
    void processBlock (const chowdsp::BufferView<float>& buffer);

private:
    Params* params = nullptr;

    std::tuple<
        chowdsp::SineWave<float>,
        chowdsp::SawtoothWave<float>,
        chowdsp::SquareWave<float>,
        chowdsp::TriangleWave<float>>
        oscillators;
    chowdsp::SmoothedBufferValue<float, juce::ValueSmoothingTypes::Multiplicative> freqParamSmoothed;
    chowdsp::Gain<float> gain;

    float nyquistHz = 48000.0f / 2.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SignalGeneratorProcessor)
};
} // namespace dsp::signal_gen
