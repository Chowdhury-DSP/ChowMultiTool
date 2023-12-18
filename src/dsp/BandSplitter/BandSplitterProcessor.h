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
        6000.0f
    };

    //for use when we have 4 bands
    //    chowdsp::FreqHzParameter::Ptr cutoff3{
    //        juce::ParameterID { "band_split_cutoff3", ParameterVersionHints::version1_0_0 },
    //        "Band Splitter Cutoff 3",
    //        chowdsp::ParamUtils::createNormalisableRange (20.0f, 20000.0f, 2000.0f),
    //        10000.0f
    //    };

    //change from bool to choice parameter when adding a 4th band
    chowdsp::BoolParameter::Ptr threeBandOnOff {
        juce::ParameterID { "band_split_3band_on", ParameterVersionHints::version1_0_0 },
        "Band Splitter 3-Band",
        false
    };
};

struct ExtraState
{
    std::atomic<bool> isEditorOpen { false };
    chowdsp::StateValue<std::atomic_bool, bool> showPreSpectrum { "band_splitter_show_pre_spectrum", true };
    chowdsp::StateValue<std::atomic_bool, bool> showPostSpectrum { "band_splitter_show_post_spectrum", true };
};

class BandSplitterProcessor
{
public:
    explicit BandSplitterProcessor (const Params& bandSplitParams, const ExtraState& extraState) : params (bandSplitParams), extraState (extraState)
    {
        midPreSpectrumAnalyserTask.SpectrumAnalyserUITask.setDBRange (-60, 5);
        midPostSpectrumAnalyserTask.SpectrumAnalyserUITask.setDBRange (-60, 5);
    }

    void prepare (const juce::dsp::ProcessSpec& spec);
    void processBlock (const chowdsp::BufferView<const float>& bufferIn,
                       const chowdsp::BufferView<float>& bufferLow,
                       const chowdsp::BufferView<float>& bufferMid,
                       const chowdsp::BufferView<float>& bufferHigh);

    std::pair<gui::SpectrumAnalyserTask::Optional, gui::SpectrumAnalyserTask::Optional> getSpectrumAnalyserTasks()
    {
        return { std::ref (midPreSpectrumAnalyserTask.SpectrumAnalyserUITask), std::ref (midPostSpectrumAnalyserTask.SpectrumAnalyserUITask) };
    }

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
    } twoBandFilters;

    struct ThreeBandFilters
    {
        chowdsp::ThreeWayCrossoverFilter<float, 1> filter1;
        chowdsp::ThreeWayCrossoverFilter<float, 2> filter2;
        chowdsp::ThreeWayCrossoverFilter<float, 4> filter4;
        chowdsp::ThreeWayCrossoverFilter<float, 8> filter8;
        chowdsp::ThreeWayCrossoverFilter<float, 12> filter12;
    } threeBandFilters;

    gui::SpectrumAnalyserTask midPreSpectrumAnalyserTask;
    gui::SpectrumAnalyserTask midPostSpectrumAnalyserTask;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandSplitterProcessor)
};
} // namespace dsp::band_splitter
