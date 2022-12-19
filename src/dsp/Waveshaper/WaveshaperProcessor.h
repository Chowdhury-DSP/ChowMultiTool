#pragma once

#include <pch.h>

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
};

struct Params : chowdsp::ParamHolder
{
    Params()
    {
        add (gainParam, shapeParam, oversampleParam);
    }

    chowdsp::GainDBParameter::Ptr gainParam {
      juce::ParameterID { "waveshaper_gain", ParameterVersionHints::version1_0_0 },
        "Waveshaper Gain",
        juce::NormalisableRange { -30.0f, 30.0f },
        0.0f
    };

    chowdsp::EnumChoiceParameter<Shapes>::Ptr shapeParam {
        juce::ParameterID { "waveshaper_shape", ParameterVersionHints::version1_0_0 },
        "Waveshaper Shape",
        Shapes::Tanh_Clip
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
    WaveshaperProcessor() = default;

    template <typename PluginParams>
    void initialise (PluginParams& pluginParams) { params = &pluginParams.waveshaperParams; }

    void prepare (const juce::dsp::ProcessSpec& spec);
    void processBlock (const chowdsp::BufferView<float>& buffer);

private:
    Params* params = nullptr;

    chowdsp::Gain<float> gain;

    using AAFilter = chowdsp::ButterworthFilter<12>;
    chowdsp::Buffer<float> upsampledBuffer;
    chowdsp::Downsampler<float, AAFilter> resample2;
    chowdsp::Downsampler<float, AAFilter> resample3;
    chowdsp::Downsampler<float, AAFilter> resample4;

    chowdsp::Downsampler<float, AAFilter>* resampler = nullptr;
    int previousUpSampleChoice = 0;

    chowdsp::SharedLookupTableCache lookupTableCache;
    chowdsp::ADAAHardClipper<float> adaaHardClipper { &lookupTableCache.get() };
    chowdsp::ADAATanhClipper<float> adaaTanhClipper { &lookupTableCache.get() };
    chowdsp::ADAASoftClipper<float> adaaCubicClipper { &lookupTableCache.get() };
    chowdsp::ADAASoftClipper<float, 9> adaa9thOrderClipper { &lookupTableCache.get() };
    chowdsp::ADAAFullWaveRectifier<float> fullWaveRectifier { &lookupTableCache.get() };
    chowdsp::WestCoastWavefolder<float> westCoastFolder { &lookupTableCache.get() };
    chowdsp::WaveMultiplier<float, 6> waveMultiplyFolder { &lookupTableCache.get() };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperProcessor)
};
}
