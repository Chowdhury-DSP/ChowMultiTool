#include "PultecPlot.h"
#include "gui/Shared/FrequencyPlotHelpers.h"

namespace gui::pultec
{
namespace
{
    constexpr double sampleRate = 48000.0f;
    constexpr int fftOrder = 15;
    constexpr int blockSize = 1 << fftOrder;
    constexpr int minFrequency = 18;
    constexpr int maxFrequency = 22'000;
} // namespace

PultecPlot::PultecPlot (State& pluginState, dsp::pultec::Params& pultecParams)
    : chowdsp::SpectrumPlotBase (chowdsp::SpectrumPlotParams {
        .minFrequencyHz = (float) minFrequency,
        .maxFrequencyHz = (float) maxFrequency,
        .minMagnitudeDB = -30.0f,
        .maxMagnitudeDB = 30.0f }),
      filterPlotter (*this, chowdsp::GenericFilterPlotter::Params {
                                .sampleRate = sampleRate,
                                .fftOrder = fftOrder,
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
    gui::drawFrequencyLines<minFrequency, maxFrequency> (*this, g);
    gui::drawMagnitudeLines (*this, g, { -30.0f, -20.0f, -10.0f, 10.0f, 20.0f, 30.0f });

    g.setColour (juce::Colours::red);
    g.strokePath (filterPlotter.getPath(), juce::PathStrokeType { 1.5f });
}

void PultecPlot::resized()
{
    updatePlot();
}
} // namespace gui::pultec
