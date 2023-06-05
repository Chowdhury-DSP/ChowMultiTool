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
    constexpr int baseKeytrackNote = 69;
} // namespace

double SVFPlot::KeytrackDotSlider::proportionOfLengthToValue (double proportion)
{
    const auto freqToMidiNote = [] (float freqHz)
    {
        return 12.0f * std::log2 (freqHz / 440.0f) + 69.0f;
    };

    const auto xPos = (double) plotBase.getWidth() * proportion;
    const auto newKeytrackFreq = plotBase.getFrequencyForXCoordinate ((float) xPos);
    return (double) freqToMidiNote (newKeytrackFreq) - (float) baseKeytrackNote;
}

double SVFPlot::KeytrackDotSlider::valueToProportionOfLength (double value)
{
    const auto keytrackFreq = dsp::svf::SVFProcessor::midiNoteToHz ((float) baseKeytrackNote + (float) value);
    const auto xPos = (double) plotBase.getXCoordinateForFrequency (keytrackFreq);
    return xPos / (double) plotBase.getWidth();
}

SVFPlot::SVFPlot (State& pluginState, dsp::svf::Params& svfParams, const chowdsp::HostContextProvider& hcp)
    : chowdsp::SpectrumPlotBase (chowdsp::SpectrumPlotParams {
        .minFrequencyHz = (float) minFrequency,
        .maxFrequencyHz = (float) maxFrequency,
        .minMagnitudeDB = -45.0f,
        .maxMagnitudeDB = 30.0f }),
      filterPlotter (*this, chowdsp::GenericFilterPlotter::Params { .sampleRate = sampleRate, .fftOrder = 15 }),
      processor (svfParams),
      freqSlider (svfParams.cutoff, pluginState, *this, SpectrumDotSlider::Orientation::FrequencyOriented, &hcp),
      keytrackSlider (svfParams.keytrackOffset, pluginState, *this, SpectrumDotSlider::Orientation::FrequencyOriented, &hcp),
      chyron (pluginState, svfParams, hcp)
{
    freqSlider.setColour (juce::Slider::thumbColourId, colours::boxColour);
    freqSlider.widthProportion = 0.03f;
    addAndMakeVisible (freqSlider);
    keytrackSlider.setColour (juce::Slider::thumbColourId, colours::boxColour);
    keytrackSlider.widthProportion = 0.03f;
    addAndMakeVisible (keytrackSlider);
    addAndMakeVisible (chyron);

    keytrackSlider.getXCoordinate = [this, &svfParams = std::as_const (svfParams)]
    {
        const auto noteWithOffset = (float) baseKeytrackNote + svfParams.keytrackOffset->get();
        const auto keytrackFreq = dsp::svf::SVFProcessor::midiNoteToHz (noteWithOffset);
        return getXCoordinateForFrequency (keytrackFreq);
    };

    processor.prepare ({ sampleRate, (uint32_t) blockSize, 1 });
    filterPlotter.runFilterCallback = [this, &svfParams = std::as_const (svfParams)] (const float* input, float* output, int numSamples)
    {
        processor.reset();
        juce::FloatVectorOperations::copy (output, input, numSamples);

        juce::MidiBuffer midi {};
        if (svfParams.keytrack->get())
        {
            midi.addEvent (juce::MidiMessage::noteOn (1, baseKeytrackNote, 1.0f), 0);
        }
        processor.processBlock (chowdsp::BufferView<float> { output, numSamples }, midi);
    };

    svfParams.doForAllParameters (
        [this, &pluginState, &svfParams = std::as_const (svfParams)] (const juce::RangedAudioParameter& param, size_t)
        {
            callbacks +=
                {
                    pluginState.addParameterListener (
                        param,
                        chowdsp::ParameterListenerThread::MessageThread,
                        [this, &param, &svfParams]
                        {
                            if (&param == svfParams.keytrack.get())
                                keytrackParamChanged (svfParams.keytrack->get());

                            updatePlot();
                        }),
                };
        });

    keytrackParamChanged (svfParams.keytrack->get());
    updatePlot();

    if (hcp.supportsParameterModulation())
        startTimerHz (29);
}

void SVFPlot::keytrackParamChanged (bool keytrackModeOn)
{
    freqSlider.setVisible (! keytrackModeOn);
    keytrackSlider.setVisible (keytrackModeOn);
    chyron.keytrackParamChanged (keytrackModeOn);
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
    drawMagnitudeLabels (g, *this, { -42.0f, -36.0f, -30.0f, -24.0f, -18.0f, -12.0f, -6.0f, 0.0f, 6.0f, 12.0f, 18.0f, 24.0f });
    drawFrequencyLabels (g, *this, { 100.0f, 1'000.0f, 10'000.0f }, 29.5f);
    gui::drawFrequencyLines<minFrequency, maxFrequency> (*this,
                                                         g,
                                                         { 100.0f, 1'000.0f, 10'000.0f },
                                                         colours::majorLinesColour,
                                                         colours::minorLinesColour);
    gui::drawMagnitudeLines (*this,
                             g,
                             { -42.0f, -36.0f, -30.0f, -24.0f, -18.0f, -12.0f, -6.0f, 0.0f, 6.0f, 12.0f, 18.0f, 24.0f },
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
    keytrackSlider.setBounds (getLocalBounds());

    const auto pad = proportionOfWidth (0.005f);
    const auto chyronWidth = proportionOfWidth (0.2f);
    const auto chyronHeight = proportionOfWidth (0.05f);
    chyron.setBounds (pad,
                      getHeight() - 5 * pad - chyronHeight,
                      chyronWidth,
                      chyronHeight);
}
} // namespace gui::svf
