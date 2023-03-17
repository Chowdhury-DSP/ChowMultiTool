#pragma once

#include <pch.h>

namespace dsp::band_splitter
{
enum class Slope
{
    m6_dBpOct = 1,
    m12_dBpOct = 2,
    m24_dBpOct = 4,
    m48_dBpOct = 8,
    m72_dBpOct = 16,
};

struct Params : chowdsp::ParamHolder
{
    Params()
    {
        add (slope, cutoff, cutoff2, threeBandOnOff);
    }

    chowdsp::EnumChoiceParameter<Slope>::Ptr slope {
        juce::ParameterID { "band_split_slope", ParameterVersionHints::version1_0_0 },
        "Band Splitter Slope",
        Slope::m12_dBpOct,
        std::initializer_list<std::pair<char, char>> { { 'm', '-' }, { '_', ' ' }, { 'p', '/' } }
    };

    chowdsp::FreqHzParameter::Ptr cutoff {
        juce::ParameterID { "band_split_cutoff", ParameterVersionHints::version1_0_0 },
        "Band Splitter Cutoff",
        chowdsp::ParamUtils::createNormalisableRange (20.0f, 20000.0f, 2000.0f),
        1000.0f
    };

    chowdsp::FreqHzParameter::Ptr cutoff2 {
        juce::ParameterID { "band_split_cutoff2", ParameterVersionHints::version1_0_0 },
        "Band Splitter Cutoff 2",
        chowdsp::ParamUtils::createNormalisableRange (20.0f, 20000.0f, 2000.0f),
        10000.0f
    };

    chowdsp::BoolParameter::Ptr threeBandOnOff {
        juce::ParameterID { "band_split_3band_on", ParameterVersionHints::version1_0_0 },
        "Band Splitter 3-Band",
        false
    };
};

class BandSplitterProcessor
{
public:
    explicit BandSplitterProcessor (Params& bandSplitParams) : params (bandSplitParams) {}

    void prepare (const juce::dsp::ProcessSpec& spec);
    void processBlock (const chowdsp::BufferView<const float>& bufferIn,
                       const chowdsp::BufferView<float>& bufferLow,
                       const chowdsp::BufferView<float>& bufferMid,
                       const chowdsp::BufferView<float>& bufferHigh);

private:
    const Params& params;

    struct TwoBandFilters
    {
        chowdsp::LinkwitzRileyFilter<float, 1> filter1;
        chowdsp::LinkwitzRileyFilter<float, 2> filter2;
        chowdsp::LinkwitzRileyFilter<float, 4> filter4;
        chowdsp::LinkwitzRileyFilter<float, 8> filter8;
        chowdsp::LinkwitzRileyFilter<float, 12> filter12;
    } twoBandFilters;

    struct ThreeBandFilters
    {
        chowdsp::ThreeWayCrossoverFilter<float, 1> filter1;
        chowdsp::ThreeWayCrossoverFilter<float, 2> filter2;
        chowdsp::ThreeWayCrossoverFilter<float, 4> filter4;
        chowdsp::ThreeWayCrossoverFilter<float, 8> filter8;
        chowdsp::ThreeWayCrossoverFilter<float, 12> filter12;
    } threeBandFilters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandSplitterProcessor)
};
} // namespace dsp::band_splitter
