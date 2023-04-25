#pragma once

#include "dsp/SignalGenerator/SignalGeneratorProcessor.h"

namespace gui::signal_gen
{
using dsp::signal_gen::Oscillator;
class OscillatorPlot : public chowdsp::SpectrumPlotBase
{
public:
    OscillatorPlot();

    void paint (juce::Graphics& g) override;

    void updatePlot (float freqParamHz, float gainParamDB, Oscillator oscillator);

private:
    float freqHz = 100.0f;
    float gainLinear = 1.0f;
    Oscillator oscChoice = Oscillator::Sine;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscillatorPlot)
};
} // namespace gui::signal_gen