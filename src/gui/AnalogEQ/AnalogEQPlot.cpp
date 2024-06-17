#include "AnalogEQPlot.h"
#include "gui/Shared/Colours.h"
#include "gui/Shared/DotSlider.h"
#include "gui/Shared/FrequencyPlotHelpers.h"
#include "gui/Shared/LookAndFeels.h"

namespace gui::analog_eq
{
namespace
{
    constexpr double sampleRate = 48000.0f;
    constexpr int fftOrder = 15;
    constexpr int blockSize = 1 << fftOrder;
    constexpr int minFrequency = 14;
    constexpr int maxFrequency = 22'000;
    constexpr auto thumbSizeFactor = 0.03f;

    struct SelectableDotSlider : SpectrumDotSlider
    {
        using SpectrumDotSlider::SpectrumDotSlider;
        bool isSelected = false;
        BandID bandID = BandID::None;
        juce::dsp::FixedSizeFunction<16, void()> callback;

        void paint (juce::Graphics& g) override
        {
            SpectrumDotSlider::paint (g);

            if (isSelected)
            {
                g.setColour (findColour (juce::Slider::thumbColourId));
                g.drawEllipse (getThumbBounds().expanded (2.0f), 1.0f);
            }
        }

        void mouseDown (const juce::MouseEvent& event) override
        {
            callback();
            SpectrumDotSlider::mouseDown (event);
        }
    };

    template <typename T, typename B, typename... Args>
    T& make_unique_component (std::unique_ptr<B>& ptr, Args&&... args)
    {
        ptr = std::make_unique<T> (std::forward<Args> (args)...);
        return static_cast<T&> (*ptr); // NOLINT
    }
} // namespace

AnalogEQPlot::BackgroundPlotter::BackgroundPlotter (chowdsp::SpectrumPlotBase& plotBase, juce::Component& parentComponent)
    : parent (parentComponent),
      filterPlotter (plotBase, chowdsp::GenericFilterPlotter::Params {
                                   .sampleRate = sampleRate,
                                   .freqSmoothOctaves = 1.0f / 12.0f,
                                   .fftOrder = fftOrder,
                               })
{
}

AnalogEQPlot::BackgroundPlotter::~BackgroundPlotter()
{
    sharedTimeSliceThread->removeTimeSliceClient (this);
    if (sharedTimeSliceThread->getNumClients() == 0)
        sharedTimeSliceThread->stopThread (-1);
}

void AnalogEQPlot::BackgroundPlotter::start()
{
    useTimeSlice();
    sharedTimeSliceThread->addTimeSliceClient (this);
    if (! sharedTimeSliceThread->isThreadRunning())
        sharedTimeSliceThread->startThread();
}

int AnalogEQPlot::BackgroundPlotter::useTimeSlice()
{
    if (chowdsp::AtomicHelpers::compareNegate (needsUpdate))
    {
        filterPlotter.updateFilterPlot();

        juce::MessageManager::callAsync (
            [safeParent = juce::Component::SafePointer { &parent }]
            {
                if (safeParent != nullptr)
                    safeParent->repaint();
            });
    }

    return 30;
}

AnalogEQPlot::AnalogEQPlot (State& pluginState,
                            dsp::analog_eq::Params& pultecParams,
                            dsp::analog_eq::ExtraState& analogEqExtraState,
                            const chowdsp::HostContextProvider& hcp,
                            SpectrumAnalyserTask::PrePostPair spectrumAnalyserTasks)
    : chowdsp::SpectrumPlotBase (chowdsp::SpectrumPlotParams {
          .minFrequencyHz = (float) minFrequency,
          .maxFrequencyHz = (float) maxFrequency,
          .minMagnitudeDB = -21.0f,
          .maxMagnitudeDB = 21.0f }),
      plotter (*this, *this),
      extraState (analogEqExtraState),
      pultecEQ (pultecParams, extraState),
      chyron (pluginState, *pluginState.params.analogEQParams, hcp),
      spectrumAnalyser (*this, spectrumAnalyserTasks)
{
    extraState.isEditorOpen.store (true);
    spectrumAnalyser.preEQDrawOptions.drawFill = true;
    spectrumAnalyser.preEQDrawOptions.drawLine = false;
    spectrumAnalyser.preEQDrawOptions.gradientStartColour = juce::Colour { 0xff282828 };
    spectrumAnalyser.preEQDrawOptions.gradientEndColour = spectrumAnalyser.preEQDrawOptions.gradientStartColour;
    spectrumAnalyser.postEQDrawOptions.drawFill = false;
    spectrumAnalyser.postEQDrawOptions.drawLine = true;
    spectrumAnalyser.postEQDrawOptions.lineColour = juce::Colour { 0xff6a6a6a };
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

    pultecEQ.prepare ({ sampleRate, (uint32_t) blockSize, 1 });
    plotter.filterPlotter.runFilterCallback = [this] (const float* input, float* output, int numSamples)
    {
        pultecEQ.reset();
        juce::FloatVectorOperations::multiply (output, input, 0.1f, numSamples);
        pultecEQ.processBlock (chowdsp::BufferView<float> { output, numSamples });
        juce::FloatVectorOperations::multiply (output, output, 10.0f, numSamples);
    };
    plotter.start();

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

    plotPainter.painter = [this] (juce::Graphics& g)
    {
        drawMagnitudeLabels (g, *this, { -20.0f, -15.0f, -10.0f, -5.0f, 0.0f, 5.0f, 10.0f, 15.0f, 20.0f });
        drawFrequencyLabels (g, *this, { 100.0f, 1000.0f, 10'000.0f }, 20.0f);

        gui::drawFrequencyLines<minFrequency, maxFrequency> (*this, g, { 100.0f, 1000.0f, 10'000.0f }, colours::majorLinesColour, colours::minorLinesColour);
        gui::drawMagnitudeLines (*this, g, { -20.0f, -15.0f, -10.0f, -5.0f, 5.0f, 10.0f, 15.0f, 20.0f }, { 0.0f }, colours::majorLinesColour, colours::minorLinesColour);

        g.setColour (juce::Colours::red);
        const juce::ScopedLock pathLock { plotter.filterPlotter.pathMutex };
        g.strokePath (plotter.filterPlotter.getPath(), juce::PathStrokeType { 1.5f });
    };
    plotPainter.setInterceptsMouseClicks (false, false);
    addAndMakeVisible (plotPainter);

    const auto setBandID = [this] (SelectableDotSlider& slider, BandID id)
    {
        slider.bandID = id;
        slider.callback = [this, id]
        { setSelectedBand (id); };
    };

    auto& lfControl = make_unique_component<SelectableDotSlider> (lowFreqControl,
                                                                  *pultecParams.bassFreqParam,
                                                                  pluginState,
                                                                  *this,
                                                                  gui::SpectrumDotSlider::FrequencyOriented,
                                                                  &hcp);
    lfControl.setColour (juce::Slider::thumbColourId, juce::Colours::goldenrod);
    lfControl.widthProportion = thumbSizeFactor;
    lfControl.bandID = BandID::Low;
    addAndMakeVisible (lfControl);

    auto& lbControl = make_unique_component<SelectableDotSlider> (lowBoostControl,
                                                                  *pultecParams.bassBoostParam,
                                                                  pluginState,
                                                                  *this,
                                                                  gui::SpectrumDotSlider::MagnitudeOriented,
                                                                  &hcp);
    lbControl.setColour (juce::Slider::thumbColourId, juce::Colours::goldenrod);
    lbControl.widthProportion = thumbSizeFactor;
    setBandID (lbControl, BandID::Low);
    lbControl.getXCoordinate = [this, &bassFreqParam = *pultecParams.bassFreqParam]
    {
        return getXCoordinateForFrequency (bassFreqParam.get() * 0.7f);
    };
    addAndMakeVisible (lbControl);

    auto& lcControl = make_unique_component<SelectableDotSlider> (lowCutControl,
                                                                  *pultecParams.bassCutParam,
                                                                  pluginState,
                                                                  *this,
                                                                  gui::SpectrumDotSlider::MagnitudeOriented,
                                                                  &hcp);
    lcControl.setColour (juce::Slider::thumbColourId, juce::Colours::goldenrod);
    lcControl.widthProportion = thumbSizeFactor;
    setBandID (lcControl, BandID::Low);
    lcControl.getXCoordinate = [this, &bassFreqParam = *pultecParams.bassFreqParam]
    {
        return getXCoordinateForFrequency (bassFreqParam.get() * 1.0f / 0.7f);
    };
    addAndMakeVisible (lcControl);

    auto& highBoostGain = make_unique_component<SelectableDotSlider> (highBoostControl,
                                                                      *pultecParams.trebleBoostParam,
                                                                      pluginState,
                                                                      *this,
                                                                      gui::SpectrumDotSlider::MagnitudeOriented);
    highBoostGain.setColour (juce::Slider::thumbColourId, juce::Colours::teal);
    highBoostGain.widthProportion = thumbSizeFactor;
    setBandID (highBoostGain, BandID::High_Boost);
    highBoostGain.getXCoordinate = [this, &trebleBoostFreqParam = *pultecParams.trebleBoostFreqParam]
    {
        return getXCoordinateForFrequency (trebleBoostFreqParam.get());
    };
    addAndMakeVisible (highBoostGain);

    auto& highBoostFreq = make_unique_component<SelectableDotSlider> (highBoostFreqControl,
                                                                      *pultecParams.trebleBoostFreqParam,
                                                                      pluginState,
                                                                      *this,
                                                                      gui::SpectrumDotSlider::FrequencyOriented);
    highBoostFreq.setColour (juce::Slider::thumbColourId, juce::Colours::teal);
    highBoostFreq.widthProportion = thumbSizeFactor;
    setBandID (highBoostFreq, BandID::High_Boost);
    highBoostFreq.getYCoordinate = [this, &trebleBoostParam = *pultecParams.trebleBoostParam]
    {
        return getYCoordinateForDecibels (trebleBoostParam.get());
    };
    addAndMakeVisible (highBoostFreq);

    auto& highBoostFull = make_unique_component<DotSliderGroup> (highBoostFullControl);
    highBoostFull.setSliders ({ &highBoostGain, &highBoostFreq });
    highBoostFull.hostContextProvider = &hcp;
    addAndMakeVisible (highBoostFull);

    auto& highCutGain = make_unique_component<SelectableDotSlider> (highCutControl,
                                                                    *pultecParams.trebleCutParam,
                                                                    pluginState,
                                                                    *this,
                                                                    gui::SpectrumDotSlider::MagnitudeOriented);
    highCutGain.setColour (juce::Slider::thumbColourId, juce::Colours::limegreen);
    highCutGain.widthProportion = thumbSizeFactor;
    setBandID (highCutGain, BandID::High_Cut);
    highCutGain.getXCoordinate = [this, &trebleCutFreqParam = *pultecParams.trebleCutFreqParam]
    {
        return getXCoordinateForFrequency (trebleCutFreqParam.get());
    };
    addAndMakeVisible (highCutGain);

    auto& highCutFreq = make_unique_component<SelectableDotSlider> (highCutFreqControl,
                                                                    *pultecParams.trebleCutFreqParam,
                                                                    pluginState,
                                                                    *this,
                                                                    gui::SpectrumDotSlider::FrequencyOriented);
    highCutFreq.setColour (juce::Slider::thumbColourId, juce::Colours::limegreen);
    highCutFreq.widthProportion = thumbSizeFactor;
    setBandID (highCutFreq, BandID::High_Cut);
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
    addAndMakeVisible (spectrumAnalyser);
    spectrumAnalyser.toBack();
}

AnalogEQPlot::~AnalogEQPlot()
{
    extraState.isEditorOpen.store (false);
}

void AnalogEQPlot::updatePlot()
{
    plotter.needsUpdate.store (true, std::memory_order_release);
}

void AnalogEQPlot::setSelectedBand (BandID band)
{
    chyron.setSelectedBand (band);
    for (juce::Slider* slider : { lowFreqControl.get(),
                                  lowBoostControl.get(),
                                  lowCutControl.get(),
                                  highBoostControl.get(),
                                  highCutControl.get(),
                                  highBoostFreqControl.get(),
                                  highCutFreqControl.get() })
    {
        auto* sliderCast = static_cast<SelectableDotSlider*> (slider); // NOLINT
        sliderCast->isSelected = sliderCast->bandID == band;
        sliderCast->repaint();
    }
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
    spectrumAnalyser.setBounds (getLocalBounds());
    plotPainter.setBounds (getLocalBounds());
}

void AnalogEQPlot::mouseDown (const juce::MouseEvent& event)
{
    setSelectedBand (BandID::None);
    if (event.mods.isPopupMenu())
    {
        chowdsp::SharedLNFAllocator lnfAllocator;
        juce::PopupMenu menu;

        juce::PopupMenu::Item preSpectrumItem;
        preSpectrumItem.itemID = 100;
        preSpectrumItem.text = extraState.showPreSpectrum.get() ? "Disable Pre-EQ Visualizer" : "Enable Pre-EQ Visualizer";
        preSpectrumItem.action = [this]
        {
            extraState.showPreSpectrum.set (! extraState.showPreSpectrum.get());
        };
        menu.addItem (preSpectrumItem);

        juce::PopupMenu::Item postSpectrumItem;
        postSpectrumItem.itemID = 101;
        postSpectrumItem.text = extraState.showPostSpectrum.get() ? "Disable Post-EQ Visualizer" : "Enable Post-EQ Visualizer";
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
} // namespace gui::analog_eq
