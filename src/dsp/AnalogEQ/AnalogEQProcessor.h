#pragma once

#include "PultecEQWDF.h"

namespace dsp::analog_eq
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

    chowdsp::GainDBParameter::Ptr bassCutParam { juce::ParameterID { "pultec_bass_cut", ParameterVersionHints::version1_0_0 },
                                                 "Analog EQ Bass Cut",
                                                 juce::NormalisableRange { -20.0f, 0.0f },
                                                 0.0f };
    chowdsp::GainDBParameter::Ptr bassBoostParam { juce::ParameterID { "pultec_bass_boost", ParameterVersionHints::version1_0_0 },
                                                   "Analog EQ Bass Boost",
                                                   juce::NormalisableRange { 0.0f, 16.0f },
                                                   0.0f };
    chowdsp::GainDBParameter::Ptr trebleCutParam { juce::ParameterID { "pultec_treble_cut", ParameterVersionHints::version1_0_0 },
                                                   "Analog EQ Treble Cut",
                                                   juce::NormalisableRange { -16.0f, 0.0f },
                                                   0.0f };
    chowdsp::GainDBParameter::Ptr trebleBoostParam { juce::ParameterID { "pultec_treble_boost", ParameterVersionHints::version1_0_0 },
                                                     "Analog EQ Treble Boost",
                                                     juce::NormalisableRange { 0.0f, 15.0f },
                                                     0.0f };
    chowdsp::PercentParameter::Ptr trebleBoostQParam { juce::ParameterID { "pultec_treble_boost_q", ParameterVersionHints::version1_0_0 },
                                                       "Analog EQ Treble Boost Q",
                                                       0.5f };

    // IMPORTANT! The frequency parameters are continuous, rather than discrete like on the original Pultec.
    // Frequencies are matched to the Pultec response, rather than the actual "truth" of the parameter value.
    // E.g: when the treble boost frequency is at 16 kHz, the EQ is actually boosting ~14.8 kHz since that
    // matches the Pultec response at that setting.

    chowdsp::FreqHzParameter::Ptr bassFreqParam {
        juce::ParameterID { "pultec_bass_freq", ParameterVersionHints::version1_0_0 },
        "Analog EQ Bass Frequency",
        chowdsp::ParamUtils::createNormalisableRange (20.0f, 100.0f, 50.0f),
        60.0f
    };

    chowdsp::FreqHzParameter::Ptr trebleCutFreqParam {
        juce::ParameterID { "pultec_treble_cut_freq", ParameterVersionHints::version1_0_0 },
        "Analog EQ Treble Cut Frequency",
        chowdsp::ParamUtils::createNormalisableRange (5000.0f, 20000.0f, 10000.0f),
        10000.0f
    };

    chowdsp::FreqHzParameter::Ptr trebleBoostFreqParam {
        juce::ParameterID { "pultec_treble_boost_freq", ParameterVersionHints::version1_0_0 },
        "Analog EQ Treble Boost Frequency",
        chowdsp::ParamUtils::createNormalisableRange (3000.0f, 16000.0f, 8000.0f),
        5000.0f
    };
};

class AnalogEQProcessor
{
public:
    explicit AnalogEQProcessor (const Params& pultecParams) : params (pultecParams) {}

    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();
    void processBlock (const chowdsp::BufferView<float>& buffer);

private:
    const Params& params;

    PultecEqWDF wdf[2];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalogEQProcessor)
};
} // namespace dsp::analog_eq
