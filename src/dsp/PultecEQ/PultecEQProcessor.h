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

    // IMPORTANT! The frequency parameters are continuous, rather than discrete like on the original Pultec.
    // Frequencies are matched to the Pultec response, rather than the actual "truth" of the parameter value.
    // E.g: when the treble boost frequency is at 16 kHz, the EQ is actually boosting ~14.8 kHz since that
    // matches the Pultec response at that setting.

    chowdsp::FreqHzParameter::Ptr bassFreqParam {
        juce::ParameterID { "pultec_bass_freq", ParameterVersionHints::version1_0_0 },
        "Pultec Bass Frequency",
        chowdsp::ParamUtils::createNormalisableRange (20.0f, 100.0f, 50.0f),
        60.0f
    };

    chowdsp::FreqHzParameter::Ptr trebleCutFreqParam {
        juce::ParameterID { "pultec_treble_cut_freq", ParameterVersionHints::version1_0_0 },
        "Pultec Treble Cut Frequency",
        chowdsp::ParamUtils::createNormalisableRange (5000.0f, 20000.0f, 10000.0f),
        10000.0f
    };

    chowdsp::FreqHzParameter::Ptr trebleBoostFreqParam {
        juce::ParameterID { "pultec_treble_boost_freq", ParameterVersionHints::version1_0_0 },
        "Pultec Treble Boost Frequency",
        chowdsp::ParamUtils::createNormalisableRange (3000.0f, 16000.0f, 8000.0f),
        5000.0f
    };
};

class PultecEQProcessor
{
public:
    explicit PultecEQProcessor (Params& pultecParams) : params (pultecParams) {}

    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();
    void processBlock (const chowdsp::BufferView<float>& buffer);

private:
    const Params& params;

    PultecEqWDF wdf[2];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PultecEQProcessor)
};
} // namespace dsp::pultec
