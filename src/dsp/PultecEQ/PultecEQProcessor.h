#pragma once

#include "PultecEQWDF.h"

namespace dsp::pultec
{
struct Params : chowdsp::ParamHolder
{
    Params()
    {
        add (bassCutParam,
             bassBoostParam,
             trebleCutParam,
             trebleBoostParam,
             trebleBoostQParam,
             bassFreqParam,
             trebleCutFreqParam,
             trebleBoostFreqParam);
    }

    chowdsp::PercentParameter::Ptr bassCutParam { juce::ParameterID { "pultec_bass_cut", ParameterVersionHints::version1_0_0 },
                                                  "Pultec Bass Cut",
                                                  0.0f };
    chowdsp::PercentParameter::Ptr bassBoostParam { juce::ParameterID { "pultec_bass_boost", ParameterVersionHints::version1_0_0 },
                                                    "Pultec Bass Boost",
                                                    0.0f };
    chowdsp::PercentParameter::Ptr trebleCutParam { juce::ParameterID { "pultec_treble_cut", ParameterVersionHints::version1_0_0 },
                                                    "Pultec Treble Cut",
                                                    0.0f };
    chowdsp::PercentParameter::Ptr trebleBoostParam { juce::ParameterID { "pultec_treble_boost", ParameterVersionHints::version1_0_0 },
                                                      "Pultec Treble Boost",
                                                      0.0f };
    chowdsp::PercentParameter::Ptr trebleBoostQParam { juce::ParameterID { "pultec_treble_boost_q", ParameterVersionHints::version1_0_0 },
                                                       "Pultec Treble Boost Q",
                                                       0.5f };

    chowdsp::ChoiceParameter::Ptr bassFreqParam {
        juce::ParameterID { "pultec_bass_freq", ParameterVersionHints::version1_0_0 },
        "Pultec Bass Frequency",
        juce::StringArray { "20 Hz", "30 Hz", "60 Hz", "100 Hz" },
        2
    };
    chowdsp::ChoiceParameter::Ptr trebleCutFreqParam {
        juce::ParameterID { "pultec_treble_cut_freq", ParameterVersionHints::version1_0_0 },
        "Pultec Treble Cut Frequency",
        juce::StringArray { "5 kHz", "10 kHz", "20 kHz" },
        1
    };
    chowdsp::ChoiceParameter::Ptr trebleBoostFreqParam {
        juce::ParameterID { "pultec_treble_boost_freq", ParameterVersionHints::version1_0_0 },
        "Pultec Treble Boost Frequency",
        juce::StringArray { "3 kHz", "4 kHz", "5 kHz", "8 kHz", "10 kHz", "12 kHz", "16 kHz" },
        2
    };
};

class PultecEQProcessor
{
public:
    PultecEQProcessor() = default;

    template <typename PluginParams>
    void initialise (PluginParams& pluginParams)
    {
        params = &pluginParams.pultecEQParams;
    }

    void prepare (const juce::dsp::ProcessSpec& spec);
    void processBlock (const chowdsp::BufferView<float>& buffer);

private:
    Params* params = nullptr;

    PultecEqWDF wdf[2];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PultecEQProcessor)
};
} // namespace dsp::pultec
