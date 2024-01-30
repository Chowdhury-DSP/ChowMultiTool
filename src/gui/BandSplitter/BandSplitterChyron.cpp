#include "BandSplitterChyron.h"

namespace gui::band_splitter
{
BandSplitterChyron::BandSplitterChyron (chowdsp::PluginState& pluginState,
                                        dsp::band_splitter::Params& bandSplitterParameters,
                                        const chowdsp::HostContextProvider& hcp)
    : state (pluginState),
      bandSplitterParams (bandSplitterParameters),
      hostContextProvider (hcp)
{
    updateValues();
    callbacks +=
        {
            pluginState.addParameterListener (*bandSplitterParams.threeBandOnOff,
                                              chowdsp::ParameterListenerThread::MessageThread,
                                              [this]
                                              {
                                                  updateValues();
                                              }),
            pluginState.addParameterListener (*bandSplitterParams.fourBandOnOff,
                                              chowdsp::ParameterListenerThread::MessageThread,
                                              [this]
                                              {
                                                  updateValues();
                                              }),
        };
}

void BandSplitterChyron::updateValues()
{
    cutoffSliderLow.reset();
    cutoffSliderMid.reset();
    cutoffSliderHigh.reset();

    auto bandState = bandSplitterParams.getCurrentBandState();
    cutoffSliderLow.emplace (state, bandSplitterParams.cutoff.get(), &hostContextProvider);
    cutoffSliderLow->setName ("Cutoff Low");
    addAndMakeVisible (*cutoffSliderLow);

    if (bandState != dsp::band_splitter::BandState::TwoBands)
    {
        cutoffSliderMid.emplace (state, bandSplitterParams.cutoff2.get(), &hostContextProvider);
        if (bandState == dsp::band_splitter::BandState::ThreeBands)
            cutoffSliderMid->setName ("Cutoff High");
        else
            cutoffSliderMid->setName ("Cutoff Mid");
        addAndMakeVisible (*cutoffSliderMid);
    }
    if (bandState == dsp::band_splitter::BandState::FourBands)
    {
        cutoffSliderHigh.emplace (state, bandSplitterParams.cutoff3.get(), &hostContextProvider);
        cutoffSliderHigh->setName ("Cutoff High");
        addAndMakeVisible (*cutoffSliderHigh);
    }
    resized();
}

void BandSplitterChyron::resized()
{
    auto bounds = getLocalBounds();
    const auto sliderBounds = bounds.withHeight (proportionOfHeight (1.0f / 3.0f));

    if (cutoffSliderMid.has_value() && cutoffSliderHigh.has_value())
    {
        const auto fourthHeight = proportionOfHeight (1.0f / 4.0f);
        cutoffSliderLow->setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), fourthHeight }).reduced (proportionOfWidth (0.025f), 0));
        cutoffSliderMid->setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), 2 * fourthHeight }).reduced (proportionOfWidth (0.025f), 0));
        cutoffSliderHigh->setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), 3 * fourthHeight }).reduced (proportionOfWidth (0.025f), 0));
    }
    else if (cutoffSliderMid.has_value())
    {
        const auto thirdHeight = proportionOfHeight (1.0f / 3.0f);
        cutoffSliderLow->setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), thirdHeight }).reduced (proportionOfWidth (0.025f), 0));
        cutoffSliderMid->setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), 2 * thirdHeight }).reduced (proportionOfWidth (0.025f), 0));
    }
    else
    {
        cutoffSliderLow->setBounds (sliderBounds.withCentre (bounds.getCentre()).reduced (proportionOfWidth (0.025f), 0));
    }
}

void BandSplitterChyron::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds();

    g.setColour (juce::Colours::black.withAlpha (0.75f));
    g.fillRoundedRectangle (bounds.toFloat(), 2.5f);

    g.setColour (colours::linesColour);
    g.drawRoundedRectangle (bounds.toFloat(), 2.5f, 1.0f);
}
} // namespace gui::band_splitter