#include "SVFPlot.h"
#include "gui/Shared/FrequencyPlotHelpers.h"

namespace gui::svf
{
namespace
{
    constexpr double sampleRate = 48000.0f;
    constexpr int fftOrder = 15;
    constexpr int blockSize = 1 << fftOrder;
    constexpr int minFrequency = 18;
    constexpr int maxFrequency = 22'000;
} // namespace

SVFPlot::SVFPlot (State& pluginState, dsp::svf::Params& svfParams)
    : chowdsp::SpectrumPlotBase (chowdsp::SpectrumPlotParams {
        .minFrequencyHz = (float) minFrequency,
        .maxFrequencyHz = (float) maxFrequency,
        .minMagnitudeDB = -30.0f,
        .maxMagnitudeDB = 30.0f }),
      filterPlotter (*this, chowdsp::GenericFilterPlotter::Params {
                                .sampleRate = sampleRate,
                                .fftOrder = 15
                            }),
      processor (svfParams)
{
    processor.prepare ({ sampleRate, (uint32_t) blockSize, 1 });
    filterPlotter.runFilterCallback = [this] (const float* input, float* output, int numSamples)
    {
        processor.reset();
        juce::FloatVectorOperations::copy (output, input, numSamples);
        processor.processBlock (chowdsp::BufferView<float> { output, numSamples });
    };

    svfParams.doForAllParameters (
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

void SVFPlot::updatePlot()
{
    filterPlotter.updateFilterPlot();
    repaint();
}

void SVFPlot::paint (juce::Graphics& g)
{
    gui::drawFrequencyLines<minFrequency, maxFrequency> (*this, g);
    gui::drawMagnitudeLines (*this, g, { -30.0f, -20.0f, -10.0f, 10.0f, 20.0f, 30.0f });

    g.setColour (juce::Colours::red);
    g.strokePath (filterPlotter.getPath(), juce::PathStrokeType { 1.5f });
}

void SVFPlot::resized()
{
    updatePlot();
}
} // namespace gui::svf
