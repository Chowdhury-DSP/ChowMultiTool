#include "AnalogEQPlot.h"
#include "gui/Shared/Colours.h"
#include "gui/Shared/DotSlider.h"
#include "gui/Shared/FrequencyPlotHelpers.h"

namespace gui::analog_eq
{
namespace
{
    constexpr double sampleRate = 48000.0f;
    constexpr int fftOrder = 15;
    constexpr int blockSize = 1 << fftOrder;
    constexpr int minFrequency = 16;
    constexpr int maxFrequency = 22'000;
} // namespace

AnalogEQPlot::AnalogEQPlot (State& pluginState, dsp::analog_eq::Params& pultecParams)
    : chowdsp::SpectrumPlotBase (chowdsp::SpectrumPlotParams {
        .minFrequencyHz = (float) minFrequency,
        .maxFrequencyHz = (float) maxFrequency,
        .minMagnitudeDB = -21.0f,
        .maxMagnitudeDB = 21.0f }),
      filterPlotter (*this, chowdsp::GenericFilterPlotter::Params {
                                .sampleRate = sampleRate,
                                .freqSmoothOctaves = 1.0f / 12.0f,
                                .fftOrder = fftOrder,
                            }),
      pultecEQ (pultecParams),
      chyron (pluginState, *pluginState.params.analogEQParams)
{
    pultecEQ.prepare ({ sampleRate, (uint32_t) blockSize, 1 });
    filterPlotter.runFilterCallback = [this] (const float* input, float* output, int numSamples)
    {
        pultecEQ.reset();
        juce::FloatVectorOperations::multiply (output, input, 0.1f, numSamples);
        pultecEQ.processBlock (chowdsp::BufferView<float> { output, numSamples });
        juce::FloatVectorOperations::multiply (output, output, 10.0f, numSamples);
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

    lowFreqControl = std::make_unique<gui::SpectrumDotSlider> (*pultecParams.bassFreqParam,
                                                               pluginState,
                                                               *this,
                                                               gui::SpectrumDotSlider::FrequencyOriented);
    lowFreqControl->setColour (juce::Slider::thumbColourId, juce::Colours::goldenrod);
    addAndMakeVisible (lowFreqControl.get());

    lowBoostControl = std::make_unique<gui::SpectrumDotSlider> (*pultecParams.bassBoostParam,
                                                                pluginState,
                                                                *this,
                                                                gui::SpectrumDotSlider::MagnitudeOriented);
    lowBoostControl->setColour (juce::Slider::thumbColourId, juce::Colours::goldenrod);
    static_cast<gui::SpectrumDotSlider*> (lowBoostControl.get())->getXCoordinate = // NOLINT
        [this, &bassFreqParam = *pultecParams.bassFreqParam]
    {
        return getXCoordinateForFrequency (bassFreqParam.get() * 0.9f);
    };
    addAndMakeVisible (lowBoostControl.get());

    lowCutControl = std::make_unique<gui::SpectrumDotSlider> (*pultecParams.bassCutParam,
                                                              pluginState,
                                                              *this,
                                                              gui::SpectrumDotSlider::MagnitudeOriented);
    lowCutControl->setColour (juce::Slider::thumbColourId, juce::Colours::goldenrod);
    static_cast<gui::SpectrumDotSlider*> (lowCutControl.get())->getXCoordinate = // NOLINT
        [this, &bassFreqParam = *pultecParams.bassFreqParam]
    {
        return getXCoordinateForFrequency (bassFreqParam.get() * 1.0f / 0.9f);
    };
    addAndMakeVisible (lowCutControl.get());

    highBoostControl = std::make_unique<gui::SpectrumDotSlider> (*pultecParams.trebleBoostParam,
                                                                 pluginState,
                                                                 *this,
                                                                 gui::SpectrumDotSlider::MagnitudeOriented);
    highBoostControl->setColour (juce::Slider::thumbColourId, juce::Colours::teal);
    static_cast<gui::SpectrumDotSlider*> (highBoostControl.get())->getXCoordinate = // NOLINT
        [this, &trebleBoostFreqParam = *pultecParams.trebleBoostFreqParam]
    {
        return getXCoordinateForFrequency (trebleBoostFreqParam.get());
    };
    addAndMakeVisible (*highBoostControl);

    highBoostFreqControl = std::make_unique<gui::SpectrumDotSlider> (*pultecParams.trebleBoostFreqParam,
                                                                     pluginState,
                                                                     *this,
                                                                     gui::SpectrumDotSlider::FrequencyOriented);
    highBoostFreqControl->setColour (juce::Slider::thumbColourId, juce::Colours::teal);

    static_cast<gui::SpectrumDotSlider*> (highBoostFreqControl.get())->getYCoordinate = // NOLINT
        [this, &trebleBoostParam = *pultecParams.trebleBoostParam]
    {
        return getYCoordinateForDecibels (trebleBoostParam.get());
    };
    addAndMakeVisible (*highBoostFreqControl);

    highBoostFullControl = std::make_unique<DotSliderGroup>();
    static_cast<DotSliderGroup*> (highBoostFullControl.get())->setSliders ({ static_cast<gui::SpectrumDotSlider*> (highBoostControl.get()), static_cast<gui::SpectrumDotSlider*> (highBoostFreqControl.get()) }); // NOLINT
    addAndMakeVisible (highBoostFullControl.get());

    highCutControl = std::make_unique<gui::SpectrumDotSlider> (*pultecParams.trebleCutParam,
                                                               pluginState,
                                                               *this,
                                                               gui::SpectrumDotSlider::MagnitudeOriented);
    highCutControl->setColour (juce::Slider::thumbColourId, juce::Colours::limegreen);
    static_cast<gui::SpectrumDotSlider*> (highCutControl.get())->getXCoordinate = // NOLINT
        [this, &trebleCutFreqParam = *pultecParams.trebleCutFreqParam]
    {
        return getXCoordinateForFrequency (trebleCutFreqParam.get());
    };
    addAndMakeVisible (highCutControl.get());

    highCutFreqControl = std::make_unique<gui::SpectrumDotSlider> (*pultecParams.trebleCutFreqParam,
                                                                   pluginState,
                                                                   *this,
                                                                   gui::SpectrumDotSlider::FrequencyOriented);
    highCutFreqControl->setColour (juce::Slider::thumbColourId, juce::Colours::limegreen);
    static_cast<gui::SpectrumDotSlider*> (highCutFreqControl.get())->getYCoordinate = // NOLINT
        [this, &trebleCutParam = *pultecParams.trebleCutParam]
    {
        return getYCoordinateForDecibels (trebleCutParam.get());
    };
    addAndMakeVisible (*highCutFreqControl);

    highCutFullControl = std::make_unique<DotSliderGroup>();
    static_cast<DotSliderGroup*> (highCutFullControl.get())->setSliders ({ static_cast<gui::SpectrumDotSlider*> (highCutControl.get()), static_cast<gui::SpectrumDotSlider*> (highCutFreqControl.get()) }); // NOLINT
    addAndMakeVisible (highCutFullControl.get());

    updatePlot();

    addAndMakeVisible (chyron);
    chyron.toFront (false);
}

AnalogEQPlot::~AnalogEQPlot() = default;

void AnalogEQPlot::updatePlot()
{
    filterPlotter.updateFilterPlot();
    repaint();
}

void AnalogEQPlot::paint (juce::Graphics& g)
{
    gui::drawFrequencyLines<minFrequency, maxFrequency> (*this, g, { 100.0f, 1000.0f, 10'000.0f }, colours::majorLinesColour, colours::minorLinesColour);
    gui::drawMagnitudeLines (*this, g, { -20.0f, -15.0f, -10.0f, -5.0f, 5.0f, 10.0f, 15.0f, 20.0f }, { 0.0f }, colours::majorLinesColour, colours::minorLinesColour);

    g.setColour (juce::Colours::red);
    g.strokePath (filterPlotter.getPath(), juce::PathStrokeType { 1.5f });
}

void AnalogEQPlot::resized()
{
    updatePlot();

    lowFreqControl->setBounds (getLocalBounds());
    lowBoostControl->setBounds (getLocalBounds());
    lowCutControl->setBounds (getLocalBounds());
    highBoostFullControl->setBounds (getLocalBounds());
    highCutFullControl->setBounds (getLocalBounds());

    const auto pad = proportionOfWidth (0.005f);
    const auto chyronWidth = proportionOfWidth (0.225f);
    const auto chyronHeight = proportionOfWidth (0.175f);
    chyron.setBounds ((int) getXCoordinateForFrequency (1000.0f) - chyronWidth / 2,
                      getHeight() - pad - chyronHeight,
                      chyronWidth,
                      chyronHeight);
}

void AnalogEQPlot::mouseDown (const juce::MouseEvent&)
{
    chyron.setSelectedBand (EQBand::None);
}
} // namespace gui::analog_eq
