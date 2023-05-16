#include "EQPlot.h"
#include "EQGUIHelpers.h"
#include "gui/Shared/Colours.h"
#include "gui/Shared/FrequencyPlotHelpers.h"

namespace gui::eq
{
juce::Rectangle<float> EQPlot::QDotSlider::getThumbBounds() const noexcept
{
    const auto dim = plotBase.getLocalBounds().proportionOfWidth (0.025f);
    return juce::Rectangle { dim, dim }
        .toFloat()
        .withCentre (juce::Point { getXCoordinate(), getYCoordinate() });
}

const auto qRange = chowdsp::ParamUtils::createNormalisableRange (0.5f, 20.0f, chowdsp::CoefficientCalculators::butterworthQ<float>);

double EQPlot::QDotSlider::proportionOfLengthToValue (double proportion)
{
    const auto ret = (double) qRange.convertFrom0to1 ((float) proportion);
    return ret;
}

double EQPlot::QDotSlider::valueToProportionOfLength (double value)
{
    return (double) qRange.convertTo0to1 ((float) value);
}

void EQPlot::EQBandSliderGroup::paint (juce::Graphics& g)
{
    if (isSelected && ! sliders.empty())
    {
        g.setColour (sliders[0]->findColour (juce::Slider::thumbColourId));
        g.drawEllipse (sliders[0]->getThumbBounds().expanded (2.0f), 1.0f);
    }
}

EQPlot::IterationsLabel::IterationsLabel (EQDrawView& view) : drawView (view)
{
    optimiserFinishedCallback = view.onCompletedOptimisation.connect ([this]
                                                                      { setVisible (false); });
}

void EQPlot::IterationsLabel::visibilityChanged()
{
    if (isVisible())
        startTimerHz (5);
    else
        stopTimer();
}

void EQPlot::IterationsLabel::timerCallback()
{
    setText ("We're Optimising... Iteration Number: " + std::to_string (drawView.getOptimiser().iterationCount), juce::dontSendNotification);
    if (drawView.getOptimiser().iterationCount > 100.0)
        setText ("Hang In There... Iteration Number: " + std::to_string (drawView.getOptimiser().iterationCount), juce::dontSendNotification);
}

constexpr int minFrequency = 18;
constexpr int maxFrequency = 22'000;

EQPlot::EQPlot (chowdsp::PluginState& pluginState,
                chowdsp::EQ::StandardEQParameters<numBands>& eqParams,
                const chowdsp::HostContextProvider& hcp)
    : chowdsp::EQ::EqualizerPlotWithParameters<numBands> (pluginState.getParameterListeners(),
                                                          eqParams,
                                                          &helpers::getFilterType,
                                                          chowdsp::SpectrumPlotParams {
                                                              .minFrequencyHz = minFrequency,
                                                              .maxFrequencyHz = maxFrequency,
                                                              .minMagnitudeDB = -23.0f,
                                                              .maxMagnitudeDB = 20.0f }),
      chyron (pluginState, eqParams, hcp),
      drawView (*this),
      eqParameters (eqParams),
      optItersLabel (drawView)
{
    for (size_t i = 0; i < numBands; ++i)
    {
        const auto setSliderActive = [] (juce::Slider& slider, bool shouldBeActive)
        {
            slider.setVisible (shouldBeActive);
        };

        freqSliders[i].emplace (*eqParameters.eqParams[i].freqParam,
                                pluginState,
                                *this,
                                SpectrumDotSlider::Orientation::FrequencyOriented);
        addChildComponent (*freqSliders[i]);
        freqSliders[i]->widthProportion = 0.03f;
        freqSliders[i]->getYCoordinate = [this, i]
        {
            return getYCoordinateForDecibels (helpers::hasGainParam (helpers::getFilterType (eqParameters.eqParams[i].typeParam->getIndex()))
                                                  ? eqParameters.eqParams[i].gainParam->get()
                                                  : 0.0f);
        };
        freqSliders[i]->setColour (juce::Slider::thumbColourId, colours::thumbColours[i]);
        setSliderActive (*freqSliders[i], eqParameters.eqParams[i].onOffParam->get());

        gainSliders[i].emplace (*eqParameters.eqParams[i].gainParam,
                                pluginState,
                                *this,
                                SpectrumDotSlider::Orientation::MagnitudeOriented);
        addChildComponent (*gainSliders[i]);
        gainSliders[i]->widthProportion = 0.03f;
        gainSliders[i]->getXCoordinate = [this, i]
        {
            return getXCoordinateForFrequency (eqParameters.eqParams[i].freqParam->get());
        };
        gainSliders[i]->setColour (juce::Slider::thumbColourId, colours::thumbColours[i]);
        setSliderActive (*gainSliders[i], eqParameters.eqParams[i].onOffParam->get() && helpers::hasGainParam (helpers::getFilterType (eqParameters.eqParams[i].typeParam->getIndex())));

        qSliders[i].emplace (*eqParameters.eqParams[i].qParam,
                             pluginState,
                             *this,
                             SpectrumDotSlider::Orientation::MagnitudeOriented);
        qSliders[i]->checkModifierKeys = [] (const juce::ModifierKeys& mods)
        { return mods.isCommandDown(); };
        qSliders[i]->setVelocityModeParameters (1.0, 1, 0.0, true, juce::ModifierKeys::altModifier);
        addChildComponent (*qSliders[i]);
        qSliders[i]->widthProportion = 0.03f;
        qSliders[i]->getYCoordinate = [this, i]
        {
            return getYCoordinateForDecibels (helpers::hasGainParam (helpers::getFilterType (eqParameters.eqParams[i].typeParam->getIndex()))
                                                  ? eqParameters.eqParams[i].gainParam->get()
                                                  : 0.0f);
        };
        qSliders[i]->getXCoordinate = [this, i]
        {
            return getXCoordinateForFrequency (eqParameters.eqParams[i].freqParam->get());
        };
        qSliders[i]->setColour (juce::Slider::thumbColourId, colours::thumbColours[i]);
        setSliderActive (*qSliders[i], eqParameters.eqParams[i].onOffParam->get() && helpers::hasQParam (helpers::getFilterType (eqParameters.eqParams[i].typeParam->getIndex())));

        callbacks += {
            pluginState.addParameterListener (*eqParameters.eqParams[i].onOffParam,
                                              chowdsp::ParameterListenerThread::MessageThread,
                                              [this, i, setSliderActive]
                                              {
                                                  const auto isOn = eqParameters.eqParams[i].onOffParam->get();
                                                  const auto filterType = helpers::getFilterType (eqParameters.eqParams[i].typeParam->getIndex());
                                                  setSliderActive (*freqSliders[i], isOn);
                                                  setSliderActive (*gainSliders[i], isOn && helpers::hasGainParam (filterType));
                                                  setSliderActive (*qSliders[i], isOn && helpers::hasQParam (filterType));
                                                  setSelectedBand (isOn ? (int) i : -1);
                                                  repaint();
                                              }),
            pluginState.addParameterListener (*eqParameters.eqParams[i].typeParam,
                                              chowdsp::ParameterListenerThread::MessageThread,
                                              [this, i, setSliderActive]
                                              {
                                                  const auto isOn = eqParameters.eqParams[i].onOffParam->get();
                                                  const auto filterType = helpers::getFilterType (eqParameters.eqParams[i].typeParam->getIndex());
                                                  setSliderActive (*gainSliders[i], isOn && helpers::hasGainParam (filterType));
                                                  setSliderActive (*qSliders[i], isOn && helpers::hasQParam (filterType));
                                                  setSelectedBand (isOn ? (int) i : -1);
                                                  repaint();
                                              }),
            pluginState.addParameterListener (*eqParameters.eqParams[i].freqParam,
                                              chowdsp::ParameterListenerThread::MessageThread,
                                              [this, i]
                                              {
                                                  setSelectedBand ((int) i);
                                                  repaint();
                                              }),
            pluginState.addParameterListener (*eqParameters.eqParams[i].qParam,
                                              chowdsp::ParameterListenerThread::MessageThread,
                                              [this, i]
                                              {
                                                  setSelectedBand ((int) i);
                                                  repaint();
                                              }),
            pluginState.addParameterListener (*eqParameters.eqParams[i].gainParam,
                                              chowdsp::ParameterListenerThread::MessageThread,
                                              [this, i]
                                              {
                                                  setSelectedBand ((int) i);
                                                  repaint();
                                              }),
        };

        sliderGroups[i].setSliders ({ &(*freqSliders[i]), &(*gainSliders[i]), &(*qSliders[i]) });
        sliderGroups[i].hostContextProvider = &hcp;
        addAndMakeVisible (sliderGroups[i]);
    }

    addAndMakeVisible (chyron);
    chyron.toFront (false);

    addChildComponent (drawView);

    setSelectedBand (-1);

    optItersLabel.setFont (juce::Font ("Georgia", 32, juce::Font::plain));
    addChildComponent (optItersLabel);
    optItersLabel.toFront (true);

    callbacks += {
        drawView.onCompletedOptimisation.connect (
            [this]
            {
                drawMode = false;
                drawView.setVisible (false);
                optItersLabel.setVisible (false);
                resized();
                repaint();
            }),
    };
}

void EQPlot::toggleDrawView (bool shouldShowDrawView, bool triggerOptimiser)
{
    if (triggerOptimiser)
    {
        optItersLabel.setVisible (true);
        drawView.triggerOptimiser (eqParameters);
        return;
    }

    drawMode = shouldShowDrawView;
    drawView.setVisible (shouldShowDrawView);

    if (drawMode) //entering draw mode
        setSelectedBand (-1);

    resized();
    repaint();
}

void EQPlot::setSelectedBand (int bandIndex)
{
    for (auto [idx, slider] : chowdsp::enumerate (sliderGroups))
    {
        slider.isSelected = (int) idx == bandIndex;
        slider.repaint();
    }
    chyron.setSelectedBand (bandIndex);
}

void EQPlot::paint (juce::Graphics& g)
{
    gui::drawFrequencyLines<minFrequency, maxFrequency> (*this,
                                                         g,
                                                         { 100.0f, 1'000.0f, 10'000.0f },
                                                         colours::majorLinesColour,
                                                         colours::minorLinesColour);
    gui::drawMagnitudeLines (*this,
                             g,
                             { -18.0f, -12.0f, -6.0f, 0.0f, 6.0f, 12.0f, 18.0f },
                             { 0.0f },
                             colours::majorLinesColour,
                             colours::minorLinesColour);

    if (! drawMode)
    {
        g.setColour (colours::linesColour);
        g.strokePath (getMasterFilterPath(), juce::PathStrokeType { 2.5f });
    }
}

void EQPlot::resized()
{
    EqualizerPlotWithParameters::resized();
    for (auto& group : sliderGroups)
        group.setBounds (drawMode ? juce::Rectangle<int> {} : getLocalBounds());

    const auto pad = proportionOfWidth (0.005f);
    const auto chyronWidth = proportionOfWidth (0.14f);
    const auto chyronHeight = proportionOfWidth (0.1f);
    chyron.setBounds (getWidth() - pad - chyronWidth,
                      getHeight() - pad - chyronHeight - proportionOfHeight (0.075f),
                      chyronWidth,
                      chyronHeight);

    drawView.setBounds (getLocalBounds());

    optItersLabel.setBounds (0, 0, 1000, 100);
}

void EQPlot::mouseDown (const juce::MouseEvent&)
{
    setSelectedBand (-1);
}
} // namespace gui::eq
