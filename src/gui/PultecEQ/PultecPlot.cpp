#include "PultecPlot.h"

namespace gui::pultec
{
namespace
{
    constexpr double sampleRate = 48000.0f;
    constexpr int fftOrder = 15;
    constexpr int blockSize = 1 << fftOrder;
    constexpr float maxFrequency = 22'000.0f;
} // namespace

PultecPlot::PultecPlot (State& pluginState, dsp::pultec::Params& pultecParams)
    : chowdsp::SpectrumPlotBase (chowdsp::SpectrumPlotParams {
        .minFrequencyHz = 18.0f,
        .maxFrequencyHz = maxFrequency,
        .minMagnitudeDB = -30.0f,
        .maxMagnitudeDB = 30.0f }),
      filterPlotter (*this, chowdsp::GenericFilterPlotter::Params {
                                .sampleRate = sampleRate,
                            }),
      pultecEQ (pultecParams)
{
    pultecEQ.prepare ({ sampleRate, (uint32_t) blockSize, 1 });
    filterPlotter.runFilterCallback = [this] (const float* input, float* output, int numSamples)
    {
        pultecEQ.reset();
        juce::FloatVectorOperations::copy (output, input, numSamples);
        pultecEQ.processBlock (chowdsp::BufferView<float> { output, numSamples });
    };

    pultecParams.doForAllParameters (
        [this, &pluginState] (juce::RangedAudioParameter& param, size_t)
        {
            callbacks +=
                {
                    pluginState.addParameterListener (
                        param,
                        chowdsp::ParameterListenerThread::MessageThread,
                        [this]
                        { updatePlot(); }),
                };
        });

    updatePlot();
}

void PultecPlot::updatePlot()
{
    filterPlotter.updateFilterPlot();
    repaint();
}

void PultecPlot::paint (juce::Graphics& g)
{
    static constexpr auto freqLines = []
    {
        std::array<float, 28> lines {};
        lines[0] = 20.0f;
        for (size_t count = 1; count < lines.size(); ++count)
        {
            const auto increment = gcem::pow (10.0f, gcem::floor (gcem::log10 (lines[count - 1])));
            lines[count] = lines[count - 1] + increment;
        }
        return lines;
    }();

    g.setColour (juce::Colours::white.withAlpha (0.25f));
    drawFrequencyLines (g, std::move (freqLines), 1.0f);

    g.setColour (juce::Colours::white.withAlpha (0.5f));
    drawFrequencyLines (g, { 100.0f, 1'000.0f, 10'000.0f }, 1.0f);

    g.setColour (juce::Colours::white.withAlpha (0.25f));
    drawMagnitudeLines (g, { -30.0f, -20.0f, -10.0f, 10.0f, 20.0f, 30.0f });

    g.setColour (juce::Colours::white.withAlpha (0.5f));
    drawMagnitudeLines (g, { 0.0f });

    g.setColour (juce::Colours::red);
    g.strokePath (filterPlotter.getPath(), juce::PathStrokeType { 1.5f });
}

void PultecPlot::resized()
{
    updatePlot();
}
} // namespace gui::pultec
