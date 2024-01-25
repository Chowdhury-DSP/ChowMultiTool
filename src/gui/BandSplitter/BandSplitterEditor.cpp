#include "BandSplitterEditor.h"
#include "gui/Shared/Colours.h"

namespace gui::band_splitter
{
BandSplitterEditor::BandSplitterEditor (State& pluginState,
                                        dsp::band_splitter::Params& params,
                                        dsp::band_splitter::ExtraState& bandSplitterExtraState,
                                        const chowdsp::HostContextProvider& hcp,
                                        dsp::band_splitter::BandSplitterSpectrumTasks& spectrumTasks)
    : bandSplitterPlot (pluginState,
                        params,
                        bandSplitterExtraState,
                        hcp,
                        spectrumTasks),
      slopePicker (pluginState, *params.slope),
      extraState (bandSplitterExtraState),
      triStateButton (pluginState)
{
    addAndMakeVisible (bandSplitterPlot);
    addAndMakeVisible (slopePicker);
    addAndMakeVisible (triStateButton);

    slopePicker.linesColour = colours::linesColour;
    slopePicker.thumbColour = colours::thumbColour;
    slopePicker.plotColour = colours::plotColour;
}

void BandSplitterEditor::paint (juce::Graphics& g)
{
    g.setGradientFill (juce::ColourGradient { colours::backgroundLight,
                                              juce::Point { 0.0f, 0.0f },
                                              colours::backgroundDark,
                                              juce::Point { (float) getWidth() * 0.35f, (float) getHeight() * 0.5f },
                                              false });
    g.fillAll();
}

void BandSplitterEditor::resized()
{
    auto bounds = getLocalBounds();
    bandSplitterPlot.setBounds (bounds);
    slopePicker.setBounds (bounds.removeFromBottom (proportionOfHeight (0.075f)));

    const auto pad = proportionOfWidth (0.005f);
    const auto dim = proportionOfWidth (0.035f);
    triStateButton.setBounds (getWidth() - pad - dim, pad, dim, dim);
}

BandSplitterEditor::TriStateButton::TriStateButton (State& pluginState) : juce::Button ("TriState"),
                                                                          triStateButtonAttachment (pluginState, *this, currentState)
{
    currentState = (pluginState.params.bandSplitParams->threeBandOnOff->get() && pluginState.params.bandSplitParams->fourBandOnOff->get()) ? std::make_pair (BandState::FourBands, 4)
                   : pluginState.params.bandSplitParams->threeBandOnOff->get()                                                             ? std::make_pair (BandState::ThreeBands, 3)
                                                                                                                                           : std::make_pair (BandState::TwoBands, 2);
}

void BandSplitterEditor::TriStateButton::paintButton (juce::Graphics& g, bool, bool)
{
    g.setColour (juce::Colours::black.withAlpha (0.75f));
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 0.1f * (float) getHeight());

    g.setFont (juce::Font { fonts->robotoBold }.withHeight ((float) getHeight()));
    g.setColour (colours::linesColour);
    const auto text = std::to_string (currentState.second);
    const auto pad = proportionOfWidth (0.2f);
    g.drawFittedText (text, getLocalBounds().reduced (pad), juce::Justification::centred, 1);
}

} // namespace gui::band_splitter
