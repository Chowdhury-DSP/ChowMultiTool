#pragma once

#include "SignalSmithWaveshaper.h"

namespace dsp::waveshaper
{
enum class Shapes
{
    Hard_Clip,
    Tanh_Clip,
    Cubic_Clip,
    Nonic_Clip,
    Full_Wave_Rectify,
    West_Coast,
    Wave_Multiply,
    Fold_Fuzz,
};

struct Params : chowdsp::ParamHolder
{
    Params()
    {
        add (gainParam, shapeParam, kParam, MParam, oversampleParam);
    }

    chowdsp::GainDBParameter::Ptr gainParam {
        juce::ParameterID { "waveshaper_gain", ParameterVersionHints::version1_0_0 },
        "Waveshaper Gain",
        juce::NormalisableRange { -12.0f, 12.0f },
        0.0f
    };

    chowdsp::EnumChoiceParameter<Shapes>::Ptr shapeParam {
        juce::ParameterID { "waveshaper_shape", ParameterVersionHints::version1_0_0 },
        "Waveshaper Shape",
        Shapes::Tanh_Clip
    };

    chowdsp::PercentParameter::Ptr kParam {
        juce::ParameterID { "waveshaper_k", ParameterVersionHints::version1_0_0 },
        "Waveshaper Fold/Fuzz Fold",
        0.5f
    };

    chowdsp::PercentParameter::Ptr MParam {
        juce::ParameterID { "waveshaper_M", ParameterVersionHints::version1_0_0 },
        "Waveshaper Fold/Fuzz Fuzz",
        0.5f
    };

    chowdsp::ChoiceParameter::Ptr oversampleParam {
        juce::ParameterID { "waveshaper_oversample", ParameterVersionHints::version1_0_0 },
        "Waveshaper Oversampling",
        juce::StringArray { "1x", "2x", "3x", "4x" },
        1
    };
};

class WaveshaperProcessor
{
public:
    explicit WaveshaperProcessor (Params& wsParams) : params (wsParams) {}

    void prepare (const juce::dsp::ProcessSpec& spec);
    void processBlock (const chowdsp::BufferView<float>& buffer);

private:
    const Params& params;

    chowdsp::Gain<float> gain;

    // TODO: oversampling

    chowdsp::Buffer<double> doubleBuffer;
    chowdsp::Buffer<xsimd::batch<double>> doubleSIMDBuffer;

    chowdsp::SharedLookupTableCache lookupTableCache;
    chowdsp::ADAAHardClipper<double> adaaHardClipper { &lookupTableCache.get() };
    chowdsp::ADAATanhClipper<double> adaaTanhClipper { &lookupTableCache.get() };
    chowdsp::ADAASoftClipper<double> adaaCubicClipper { &lookupTableCache.get() };
    chowdsp::ADAASoftClipper<double, 9> adaa9thOrderClipper { &lookupTableCache.get() };
    chowdsp::ADAAFullWaveRectifier<double> fullWaveRectifier { &lookupTableCache.get() };
    chowdsp::WestCoastWavefolder<double> westCoastFolder { &lookupTableCache.get() };
    chowdsp::WaveMultiplier<double, 6> waveMultiplyFolder { &lookupTableCache.get() };
    SignalSmithWaveshaper ssWaveshaper;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperProcessor)
};
} // namespace dsp::waveshaper
