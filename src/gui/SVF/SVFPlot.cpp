#include "SVFPlot.h"
#include "gui/Shared/Colours.h"
#include "gui/Shared/FrequencyPlotHelpers.h"

namespace gui::svf
{
namespace
{
    constexpr double sampleRate = 48000.0f;
    constexpr int fftOrder = 16;
    constexpr int blockSize = 1 << fftOrder;
    constexpr int minFrequency = 18;
    constexpr int maxFrequency = 20'200;
} // namespace

SVFPlot::SVFPlot (State& pluginState, dsp::svf::Params& svfParams, const chowdsp::HostContextProvider& hcp)
    : chowdsp::SpectrumPlotBase (chowdsp::SpectrumPlotParams {
        .minFrequencyHz = (float) minFrequency,
        .maxFrequencyHz = (float) maxFrequency,
        .minMagnitudeDB = -45.0f,
        .maxMagnitudeDB = 30.0f }),
      filterPlotter (*this, chowdsp::GenericFilterPlotter::Params { .sampleRate = sampleRate, .fftOrder = 15 }),
      processor (svfParams),
      freqSlider (svfParams.cutoff, pluginState, *this, SpectrumDotSlider::Orientation::FrequencyOriented, &hcp)
{
    addAndMakeVisible (freqSlider);

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

    if (hcp.supportsParameterModulation())
        startTimerHz (29);
}

void SVFPlot::timerCallback()
{
    updatePlot();
}

void SVFPlot::updatePlot()
{
    filterPlotter.updateFilterPlot();
    repaint();
}

void SVFPlot::paint (juce::Graphics& g)
{
    gui::drawFrequencyLines<minFrequency, maxFrequency> (*this,
                                                         g,
                                                         { 100.0f, 1'000.0f, 10'000.0f },
                                                         colours::majorLinesColour,
                                                         colours::minorLinesColour);
    gui::drawMagnitudeLines (*this,
                             g,
                             { -42.0f, -36.0f, -24.0f, -18.0f, -12.0f, -6.0f, 0.0f, 6.0f, 12.0f, 18.0f },
                             { 0.0f },
                             colours::majorLinesColour,
                             colours::minorLinesColour);

    g.setColour (colours::plotColour);
    g.strokePath (filterPlotter.getPath(), juce::PathStrokeType { 2.5f });
}

void SVFPlot::resized()
{
    updatePlot();
    freqSlider.setBounds (getLocalBounds());
}
} // namespace gui::svf
