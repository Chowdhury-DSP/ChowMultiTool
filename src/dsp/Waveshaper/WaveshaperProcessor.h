#pragma once

#include "SignalSmithWaveshaper.h"
#include "SplineWaveshaper.h"

namespace dsp::waveshaper
{
enum class Shapes
{
    Hard_Clip = 1,
    Tanh_Clip = 2,
    Cubic_Clip = 4,
    Nonic_Clip = 8,
    Full_Wave_Rectify = 16,
    West_Coast = 32,
    Wave_Multiply = 64,
    Fold_Fuzz = 128,
    Free_Draw = 256,
    Math = 512,
    Spline = 1024,
};

enum class OversamplingRatio
{
    OneX = 1,
    TwoX = 2,
    FourX = 4,
    EightX = 8,
};
} // namespace dsp::waveshaper

template <>
constexpr magic_enum::customize::customize_t
    magic_enum::customize::enum_name<dsp::waveshaper::OversamplingRatio> (dsp::waveshaper::OversamplingRatio value) noexcept
{
    using dsp::waveshaper::OversamplingRatio;
    switch (value)
    {
        case OversamplingRatio::OneX:
            return "1x";
        case OversamplingRatio::TwoX:
            return "2x";
        case OversamplingRatio::FourX:
            return "4x";
        case OversamplingRatio::EightX:
            return "8x";
    }
    return default_tag;
}

namespace dsp::waveshaper
{
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

    chowdsp::EnumChoiceParameter<OversamplingRatio>::Ptr oversampleParam {
        juce::ParameterID { "waveshaper_oversample", ParameterVersionHints::version1_0_0 },
        "Waveshaper Oversampling",
        OversamplingRatio::TwoX
    };
};

struct ExtraState
{
    spline::SplineState freeDrawState { "waveshaper_free_draw", spline::DefaultSplineCreator {} };
    spline::SplineState mathState { "waveshaper_math", spline::DefaultSplineCreator {} };
    spline::VectorSplineState pointsState { "waveshaper_points", spline::DefaultVectorSplineCreator {} };
};

class WaveshaperProcessor
{
public:
    WaveshaperProcessor (chowdsp::PluginState& state, Params& wsParams, ExtraState& wsExtraState);

    void prepare (const juce::dsp::ProcessSpec& spec);
    void processBlock (const chowdsp::BufferView<float>& buffer);

private:
    void oversamplingRateChanged();

    const Params& params;
    chowdsp::ScopedCallback osChangeCallback;

    juce::dsp::ProcessSpec processSpec {};
    juce::SpinLock processingMutex;

    chowdsp::Gain<float> gain;

    using AAFilter = chowdsp::EllipticFilter<8>;
    chowdsp::Upsampler<float, AAFilter> upsampler;
    chowdsp::Downsampler<float, AAFilter> downsampler;

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
    spline::SplineWaveshaper<spline::SplinePoints, spline::SplineADAA> freeDrawShaper;
    spline::SplineWaveshaper<spline::SplinePoints, spline::SplineADAA> mathShaper;
    spline::SplineWaveshaper<spline::VectorSplinePoints, spline::VectorSplineADAA> pointsShaper;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperProcessor)
};
} // namespace dsp::waveshaper
