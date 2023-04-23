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

    template <typename T, typename B, typename... Args>
    T& make_unique_component (std::unique_ptr<B>& ptr, Args&&... args)
    {
        ptr = std::make_unique<T> (std::forward<Args> (args)...);
        return static_cast<T&> (*ptr); // NOLINT
    }
} // namespace

AnalogEQPlot::AnalogEQPlot (State& pluginState, dsp::analog_eq::Params& pultecParams, const chowdsp::HostContextProvider& hcp)
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
      chyron (pluginState, *pluginState.params.analogEQParams, hcp)
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

    auto& lfControl = make_unique_component<gui::SpectrumDotSlider> (lowFreqControl,
                                                                     *pultecParams.bassFreqParam,
                                                                     pluginState,
                                                                     *this,
                                                                     gui::SpectrumDotSlider::FrequencyOriented,
                                                                     &hcp);
    lfControl.setColour (juce::Slider::thumbColourId, juce::Colours::goldenrod);
    addAndMakeVisible (lfControl);

    auto& lbControl = make_unique_component<gui::SpectrumDotSlider> (lowBoostControl,
                                                                     *pultecParams.bassBoostParam,
                                                                     pluginState,
                                                                     *this,
                                                                     gui::SpectrumDotSlider::MagnitudeOriented,
                                                                     &hcp);
    lbControl.setColour (juce::Slider::thumbColourId, juce::Colours::goldenrod);
    lbControl.getXCoordinate = [this, &bassFreqParam = *pultecParams.bassFreqParam]
    {
        return getXCoordinateForFrequency (bassFreqParam.get() * 0.9f);
    };
    addAndMakeVisible (lbControl);

    auto& lcControl = make_unique_component<gui::SpectrumDotSlider> (lowCutControl,
                                                                     *pultecParams.bassCutParam,
                                                                     pluginState,
                                                                     *this,
                                                                     gui::SpectrumDotSlider::MagnitudeOriented,
                                                                     &hcp);
    lcControl.setColour (juce::Slider::thumbColourId, juce::Colours::goldenrod);
    lcControl.getXCoordinate = [this, &bassFreqParam = *pultecParams.bassFreqParam]
    {
        return getXCoordinateForFrequency (bassFreqParam.get() * 1.0f / 0.9f);
    };
    addAndMakeVisible (lcControl);

    auto& highBoostGain = make_unique_component<gui::SpectrumDotSlider> (highBoostControl,
                                                                         *pultecParams.trebleBoostParam,
                                                                         pluginState,
                                                                         *this,
                                                                         gui::SpectrumDotSlider::MagnitudeOriented);
    highBoostGain.setColour (juce::Slider::thumbColourId, juce::Colours::teal);
    highBoostGain.getXCoordinate = [this, &trebleBoostFreqParam = *pultecParams.trebleBoostFreqParam]
    {
        return getXCoordinateForFrequency (trebleBoostFreqParam.get());
    };
    addAndMakeVisible (highBoostGain);

    auto& highBoostFreq = make_unique_component<gui::SpectrumDotSlider> (highBoostFreqControl,
                                                                         *pultecParams.trebleBoostFreqParam,
                                                                         pluginState,
                                                                         *this,
                                                                         gui::SpectrumDotSlider::FrequencyOriented);
    highBoostFreq.setColour (juce::Slider::thumbColourId, juce::Colours::teal);
    highBoostFreq.getYCoordinate = [this, &trebleBoostParam = *pultecParams.trebleBoostParam]
    {
        return getYCoordinateForDecibels (trebleBoostParam.get());
    };
    addAndMakeVisible (highBoostFreq);

    auto& highBoostFull = make_unique_component<DotSliderGroup> (highBoostFullControl);
    highBoostFull.setSliders ({ &highBoostGain, &highBoostFreq });
    highBoostFull.hostContextProvider = &hcp;
    addAndMakeVisible (highBoostFull);

    auto& highCutGain = make_unique_component<gui::SpectrumDotSlider> (highCutControl,
                                                                       *pultecParams.trebleCutParam,
                                                                       pluginState,
                                                                       *this,
                                                                       gui::SpectrumDotSlider::MagnitudeOriented);
    highCutGain.setColour (juce::Slider::thumbColourId, juce::Colours::limegreen);
    highCutGain.getXCoordinate = [this, &trebleCutFreqParam = *pultecParams.trebleCutFreqParam]
    {
        return getXCoordinateForFrequency (trebleCutFreqParam.get());
    };
    addAndMakeVisible (highCutGain);

    auto& highCutFreq = make_unique_component<gui::SpectrumDotSlider> (highCutFreqControl,
                                                                       *pultecParams.trebleCutFreqParam,
                                                                       pluginState,
                                                                       *this,
                                                                       gui::SpectrumDotSlider::FrequencyOriented);
    highCutFreq.setColour (juce::Slider::thumbColourId, juce::Colours::limegreen);
    highCutFreq.getYCoordinate = [this, &trebleCutParam = *pultecParams.trebleCutParam]
    {
        return getYCoordinateForDecibels (trebleCutParam.get());
    };
    addAndMakeVisible (highCutFreq);

    auto& highCutFull = make_unique_component<DotSliderGroup> (highCutFullControl);
    highCutFull.setSliders ({ &highCutGain, &highCutFreq });
    highCutFull.hostContextProvider = &hcp;
    addAndMakeVisible (highCutFull);

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
