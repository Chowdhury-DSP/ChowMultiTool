#include "OscillatorPlot.h"
#include "gui/Shared/Colours.h"
#include "gui/Shared/FrequencyPlotHelpers.h"

namespace gui::signal_gen
{
constexpr int minFrequency = 10;
constexpr int maxFrequency = 24000;

OscillatorPlot::OscillatorPlot()
    : chowdsp::SpectrumPlotBase (chowdsp::SpectrumPlotParams {
        .minFrequencyHz = (float) minFrequency,
        .maxFrequencyHz = (float) maxFrequency,
        .minMagnitudeDB = -80.0f,
        .maxMagnitudeDB = 6.0f,
    })
{
}

void OscillatorPlot::paint (juce::Graphics& g)
{
    gui::drawFrequencyLines<minFrequency, maxFrequency> (*this, g, { 100.0f, 1'000.0f, 10'000.0f }, colours::majorLinesColour, colours::minorLinesColour);
    gui::drawMagnitudeLines (*this, g, { -72.0f, -60.0f, -48.0f, -36.0f, -24.0f, -12.0f, 0.0f, 12.0f, 24.0f }, { 0.0f }, colours::majorLinesColour, colours::minorLinesColour);

    g.setColour (colours::plotColour);
    for (float freqMult = 1.0f; freqMult * freqHz < params.maxFrequencyHz; freqMult += 1.0f)
    {
        const auto gain = [this, freqMult]
        {
            switch (oscChoice)
            {
                case Oscillator::Sine:
                    return freqMult == 1.0f ? gainLinear : 0.0f;
                case Oscillator::Saw:
                    return gainLinear / freqMult;
                case Oscillator::Square:
                    return juce::roundToInt (freqMult) % 2 == 1 ? gainLinear / freqMult : 0.0f;
                case Oscillator::Triangle:
                    return juce::roundToInt (freqMult) % 2 == 1 ? gainLinear / chowdsp::Power::ipow<2> (freqMult) : 0.0f;
                default:
                    return 0.0f;
            }
        }();

        if (gain == 0.0f)
            continue;

        const auto xCoord = getXCoordinateForFrequency (freqHz * freqMult);
        const auto yCoord = getYCoordinateForDecibels (juce::Decibels::gainToDecibels (gain));
        g.drawLine (xCoord, yCoord, xCoord, (float) getHeight(), 2.0f);
    }
}

void OscillatorPlot::updatePlot (float freqParamHz, float gainParamDB, Oscillator oscillator)
{
    freqHz = freqParamHz;
    gainLinear = juce::Decibels::decibelsToGain (gainParamDB);
    oscChoice = oscillator;
    repaint();
}
} // namespace gui::signal_gen
