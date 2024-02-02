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
    cutoffSlider1.reset();
    cutoffSlider2.reset();
    cutoffSlider3.reset();

    auto bandState = bandSplitterParams.getCurrentBandState();
    cutoffSlider1.emplace (state, bandSplitterParams.cutoff.get(), &hostContextProvider);
    if (bandState == dsp::band_splitter::BandState::TwoBands)
        cutoffSlider1->setName ("Cutoff");
    else
        cutoffSlider1->setName ("Cutoff Low");
    addAndMakeVisible (*cutoffSlider1);

    if (bandState != dsp::band_splitter::BandState::TwoBands)
    {
        cutoffSlider2.emplace (state, bandSplitterParams.cutoff2.get(), &hostContextProvider);
        if (bandState == dsp::band_splitter::BandState::ThreeBands)
            cutoffSlider2->setName ("Cutoff High");
        else
            cutoffSlider2->setName ("Cutoff Mid");
        addAndMakeVisible (*cutoffSlider2);
    }
    if (bandState == dsp::band_splitter::BandState::FourBands)
    {
        cutoffSlider3.emplace (state, bandSplitterParams.cutoff3.get(), &hostContextProvider);
        cutoffSlider3->setName ("Cutoff High");
        addAndMakeVisible (*cutoffSlider3);
    }
    resized();
}

void BandSplitterChyron::resized()
{
    auto bounds = getLocalBounds();
    const auto sliderBounds = bounds.withHeight (proportionOfHeight (1.0f / 3.0f));

    if (cutoffSlider2.has_value() && cutoffSlider3.has_value())
    {
        const auto fourthHeight = proportionOfHeight (1.0f / 4.0f);
        cutoffSlider1->setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), fourthHeight }).reduced (proportionOfWidth (0.025f), 0));
        cutoffSlider2->setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), 2 * fourthHeight }).reduced (proportionOfWidth (0.025f), 0));
        cutoffSlider3->setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), 3 * fourthHeight }).reduced (proportionOfWidth (0.025f), 0));
    }
    else if (cutoffSlider2.has_value())
    {
        const auto thirdHeight = proportionOfHeight (1.0f / 3.0f);
        cutoffSlider1->setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), thirdHeight }).reduced (proportionOfWidth (0.025f), 0));
        cutoffSlider2->setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), 2 * thirdHeight }).reduced (proportionOfWidth (0.025f), 0));
    }
    else
    {
        cutoffSlider1->setBounds (sliderBounds.withCentre (bounds.getCentre()).reduced (proportionOfWidth (0.025f), 0));
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