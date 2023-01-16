#pragma once

#include <pch.h>

namespace dsp::band_splitter
{
enum class Slope
{
    m6_dBpOct,
    m12_dBpOct,
    m24_dBpOct,
    m48_dBpOct,
    m96_dBpOct,
};

struct Params : chowdsp::ParamHolder
{
    Params()
    {
        add (slope, cutoff);
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
};

class BandSplitterProcessor
{
public:
    BandSplitterProcessor() = default;

    template <typename PluginParams>
    void initialise (PluginParams& pluginParams)
    {
        params = &pluginParams.bandSplitParams;
    }

    void prepare (const juce::dsp::ProcessSpec& spec);
    void processBlock (const chowdsp::BufferView<const float>& bufferIn,
                       const chowdsp::BufferView<float>& bufferLow,
                       const chowdsp::BufferView<float>& bufferHigh);

private:
    Params* params = nullptr;

    chowdsp::LinkwitzRileyFilter<float, 1> filter1;
    chowdsp::LinkwitzRileyFilter<float, 2> filter2;
    chowdsp::LinkwitzRileyFilter<float, 4> filter4;
    chowdsp::LinkwitzRileyFilter<float, 8> filter8;
    chowdsp::LinkwitzRileyFilter<float, 12> filter12;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandSplitterProcessor)
};
} // namespace dsp::band_splitter
