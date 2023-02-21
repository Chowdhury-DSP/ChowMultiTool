#include "PultecPlot.h"
#include "gui/Shared/DotSlider.h"
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
        .minMagnitudeDB = -20.0f,
        .maxMagnitudeDB = 20.0f }),
      filterPlotter (*this, chowdsp::GenericFilterPlotter::Params {
                                .sampleRate = sampleRate,
                                .freqSmoothOctaves = 1.0f / 12.0f,
                                .fftOrder = fftOrder,
                            }),
      pultecEQ (pultecParams)
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
        [this, &bassFreqParam = *pultecParams.bassFreqParam]
    {
        return getXCoordinateForFrequency (8000.0f);
    };
    addAndMakeVisible (highBoostControl.get());

    highCutControl = std::make_unique<gui::SpectrumDotSlider> (*pultecParams.trebleCutParam,
                                                               pluginState,
                                                               *this,
                                                               gui::SpectrumDotSlider::MagnitudeOriented);
    highCutControl->setColour (juce::Slider::thumbColourId, juce::Colours::limegreen);
    static_cast<gui::SpectrumDotSlider*> (highCutControl.get())->getXCoordinate = // NOLINT
        [this, &bassFreqParam = *pultecParams.bassFreqParam]
    {
        return getXCoordinateForFrequency (12000.0f);
    };
    addAndMakeVisible (highCutControl.get());

    updatePlot();
}

PultecPlot::~PultecPlot() = default;

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

    lowFreqControl->setBounds (getLocalBounds());
    lowBoostControl->setBounds (getLocalBounds());
    lowCutControl->setBounds (getLocalBounds());
    highBoostControl->setBounds (getLocalBounds());
    highCutControl->setBounds (getLocalBounds());
}
} // namespace gui::pultec
