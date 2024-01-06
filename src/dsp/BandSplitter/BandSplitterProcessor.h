#pragma once

#include "gui/Shared/SpectrumAnalyserTask.h"
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
        add (slope, cutoff, cutoff2, cutoff3, threeBandOnOff, fourBandOnOff);
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
        6000.0f
    };

    chowdsp::FreqHzParameter::Ptr cutoff3 {
        juce::ParameterID { "band_split_cutoff3", ParameterVersionHints::version1_0_0 },
        "Band Splitter Cutoff 3",
        chowdsp::ParamUtils::createNormalisableRange (20.0f, 20000.0f, 2000.0f),
        10000.0f
    };

    chowdsp::BoolParameter::Ptr threeBandOnOff {
        juce::ParameterID { "band_split_3band_on", ParameterVersionHints::version1_0_0 },
        "Band Splitter 3-Band",
        false
    };

    chowdsp::BoolParameter::Ptr fourBandOnOff {
        juce::ParameterID { "band_split_4band_on", ParameterVersionHints::version1_0_0 },
        "Band Splitter 4-Band",
        false
    };
};

struct ExtraState
{
    std::atomic<bool> isEditorOpen { false };
    chowdsp::StateValue<std::atomic_bool, bool> showSpectrum { "band_splitter_show_spectrum", true };
};

enum class SpectrumBandID : size_t
{
    Low = 0,
    Mid,
    High,
    LowMid,
    HighMid,
};
using BandSplitterSpectrumTasks = chowdsp::SmallMap<SpectrumBandID, gui::SpectrumAnalyserTask*>;

class BandSplitterProcessor
{
public:
    BandSplitterProcessor (const Params& bandSplitParams, const ExtraState& extraState);

    void prepare (const juce::dsp::ProcessSpec& spec);
    void processBlock (const chowdsp::BufferView<const float>& bufferIn,
                       const chowdsp::BufferView<float>& bufferLow,
                       const chowdsp::BufferView<float>& bufferMid,
                       const chowdsp::BufferView<float>& bufferHigh,
                       const chowdsp::BufferView<float>& bufferLowMid,
                       const chowdsp::BufferView<float>& bufferHighMid);

    BandSplitterSpectrumTasks& getAnalyzerTasks() { return analyzerTasks; }

private:
    const Params& params;
    const ExtraState& extraState;

    struct TwoBandFilters
    {
        chowdsp::LinkwitzRileyFilter<float, 1> filter1;
        chowdsp::LinkwitzRileyFilter<float, 2> filter2;
        chowdsp::LinkwitzRileyFilter<float, 4> filter4;
        chowdsp::LinkwitzRileyFilter<float, 8> filter8;
        chowdsp::LinkwitzRileyFilter<float, 12> filter12;
    } twoBandFilters {};

    struct ThreeBandFilters
    {
        chowdsp::ThreeWayCrossoverFilter<float, 1> filter1;
        chowdsp::ThreeWayCrossoverFilter<float, 2> filter2;
        chowdsp::ThreeWayCrossoverFilter<float, 4> filter4;
        chowdsp::ThreeWayCrossoverFilter<float, 8> filter8;
        chowdsp::ThreeWayCrossoverFilter<float, 12> filter12;
    } threeBandFilters {};

    struct FourBandFiltersStage1
    {
        chowdsp::LinkwitzRileyFilter<float, 1> filter1;
        chowdsp::LinkwitzRileyFilter<float, 2> filter2;
        chowdsp::LinkwitzRileyFilter<float, 4> filter4;
        chowdsp::LinkwitzRileyFilter<float, 8> filter8;
        chowdsp::LinkwitzRileyFilter<float, 12> filter12;
    } fourBandFiltersStage1 {};

    struct FourBandFiltersLowStage2
    {
        chowdsp::LinkwitzRileyFilter<float, 1> filter1;
        chowdsp::LinkwitzRileyFilter<float, 2> filter2;
        chowdsp::LinkwitzRileyFilter<float, 4> filter4;
        chowdsp::LinkwitzRileyFilter<float, 8> filter8;
        chowdsp::LinkwitzRileyFilter<float, 12> filter12;
    } fourBandFiltersLowStage2 {};

    struct FourBandFiltersHighStage2
    {
        chowdsp::LinkwitzRileyFilter<float, 1> filter1;
        chowdsp::LinkwitzRileyFilter<float, 2> filter2;
        chowdsp::LinkwitzRileyFilter<float, 4> filter4;
        chowdsp::LinkwitzRileyFilter<float, 8> filter8;
        chowdsp::LinkwitzRileyFilter<float, 12> filter12;
    } fourBandFiltersHighStage2 {};

    gui::SpectrumAnalyserTask lowSpectrumAnalyserTask;
    gui::SpectrumAnalyserTask midSpectrumAnalyserTask;
    gui::SpectrumAnalyserTask highSpectrumAnalyserTask;
    gui::SpectrumAnalyserTask lowMidSpectrumAnalyserTask;
    gui::SpectrumAnalyserTask highMidSpectrumAnalyserTask;
    BandSplitterSpectrumTasks analyzerTasks {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandSplitterProcessor)
};
} // namespace dsp::band_splitter
