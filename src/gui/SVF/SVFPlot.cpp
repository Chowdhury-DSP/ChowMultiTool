#include "SVFPlot.h"

namespace gui::svf
{
namespace
{
    constexpr double sampleRate = 48000.0f;
    constexpr int fftOrder = 15;
    constexpr int blockSize = 1 << fftOrder;
    constexpr float maxFrequency = 22'000.0f;
} // namespace

SVFPlot::SVFPlot (State& pluginState, dsp::svf::Params& svfParams)
    : chowdsp::SpectrumPlotBase (chowdsp::SpectrumPlotParams {
        .minFrequencyHz = 18.0f,
        .maxFrequencyHz = maxFrequency,
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

void SVFPlot::resized()
{
    updatePlot();
}
} // namespace gui::svf
