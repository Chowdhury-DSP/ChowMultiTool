#include "SVFPlot.h"
#include "gui/Shared/Colours.h"
#include "gui/Shared/FrequencyPlotHelpers.h"
#include "gui/Shared/LookAndFeels.h"

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

SVFPlot::SVFPlot (State& pluginState,
                  dsp::svf::Params& svfParams,
                  dsp::svf::ExtraState& svfExtraState,
                  const chowdsp::HostContextProvider& hcp,
                  std::pair<gui::SpectrumAnalyserTask::OptionalBackgroundTask, gui::SpectrumAnalyserTask::OptionalBackgroundTask> spectrumAnalyserTasks)
    : chowdsp::SpectrumPlotBase (chowdsp::SpectrumPlotParams {
        .minFrequencyHz = (float) minFrequency,
        .maxFrequencyHz = (float) maxFrequency,
        .minMagnitudeDB = -45.0f,
        .maxMagnitudeDB = 30.0f }),
      filterPlotter (*this, chowdsp::GenericFilterPlotter::Params { .sampleRate = sampleRate, .fftOrder = 15 }),
      extraState (svfExtraState),
      processor (svfParams, extraState),
      freqSlider (svfParams.cutoff, pluginState, *this, SpectrumDotSlider::Orientation::FrequencyOriented, &hcp),
      keytrackSlider (svfParams.keytrackOffset, pluginState, *this, SpectrumDotSlider::Orientation::FrequencyOriented, &hcp),
      spectrumAnalyser (*this, spectrumAnalyserTasks),
      chyron (pluginState, svfParams, hcp)
{
    addMouseListener (this, true);
    extraState.isEditorOpen.store (true);
    spectrumAnalyser.setShouldShowPreEQ (extraState.showPreSpectrum.get());
    spectrumAnalyser.setShouldShowPostEQ (extraState.showPostSpectrum.get());
    callbacks += {
        extraState.showPreSpectrum.changeBroadcaster.connect ([this]
                                                              {
                                                                  spectrumAnalyser.setShouldShowPreEQ(extraState.showPreSpectrum.get());
                                                                  spectrumAnalyser.repaint(); }),
        extraState.showPostSpectrum.changeBroadcaster.connect ([this]
                                                               {
                                                                   spectrumAnalyser.setShouldShowPostEQ(extraState.showPostSpectrum.get());
                                                                   spectrumAnalyser.repaint(); }),
    };

    freqSlider.setColour (juce::Slider::thumbColourId, colours::boxColour);
    freqSlider.widthProportion = 0.03f;
    addAndMakeVisible (freqSlider);
    keytrackSlider.setColour (juce::Slider::thumbColourId, colours::boxColour);
    keytrackSlider.widthProportion = 0.03f;
    addAndMakeVisible (keytrackSlider);
    addAndMakeVisible (spectrumAnalyser);
    addAndMakeVisible (chyron);
    spectrumAnalyser.toBack();

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

SVFPlot::~SVFPlot()
{
    extraState.isEditorOpen.store (false);
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
}

void SVFPlot::paintOverChildren (juce::Graphics& g)
{
    g.setColour (colours::plotColour);
    g.strokePath (filterPlotter.getPath(), juce::PathStrokeType { 2.5f });
}

void SVFPlot::resized()
{
    updatePlot();
    spectrumAnalyser.setBounds (getLocalBounds());
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

void SVFPlot::mouseDown (const juce::MouseEvent& event)
{
    if (event.mods.isPopupMenu())
    {
        chowdsp::SharedLNFAllocator lnfAllocator;
        juce::PopupMenu menu;

        juce::PopupMenu::Item preSpectrumItem;
        preSpectrumItem.itemID = 100;
        preSpectrumItem.text = extraState.showPreSpectrum.get() ? "Disable Pre-Filter Visualizer" : "Enable Pre-Filter Visualizer";
        preSpectrumItem.action = [this]
        {
            extraState.showPreSpectrum.set (! extraState.showPreSpectrum.get());
        };
        menu.addItem (preSpectrumItem);

        juce::PopupMenu::Item postSpectrumItem;
        postSpectrumItem.itemID = 101;
        postSpectrumItem.text = extraState.showPostSpectrum.get() ? "Disable Post-Filter Visualizer" : "Enable Post-Filter Visualizer";
        postSpectrumItem.action = [this]
        {
            extraState.showPostSpectrum.set (! extraState.showPostSpectrum.get());
        };
        menu.addItem (postSpectrumItem);

        menu.setLookAndFeel (lnfAllocator->getLookAndFeel<lnf::MenuLNF>());
        menu.showMenuAsync (juce::PopupMenu::Options {}
                                .withParentComponent (getParentComponent()));
    }
}

} // namespace gui::svf
