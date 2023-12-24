#include "BandSplitterEditor.h"
#include "gui/Shared/Colours.h"

namespace gui::band_splitter
{
BandSplitterEditor::BandsButton::BandsButton (chowdsp::BoolParameter& param, State& pluginState)
    : juce::Button ("Bands"),
      attach (param, pluginState, *this),
      bandParam (param)
{
    setClickingTogglesState (true);
}

void BandSplitterEditor::BandsButton::paintButton (juce::Graphics& g, bool, bool)
{
    g.setColour (juce::Colours::black.withAlpha (0.75f));
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 0.1f * (float) getHeight());

    g.setFont (juce::Font { fonts->robotoBold }.withHeight ((float) getHeight()));
    g.setColour (colours::linesColour);
    const auto text = bandParam.get() ? "3" : "2";
    const auto pad = proportionOfWidth (0.2f);
    g.drawFittedText (text, getLocalBounds().reduced (pad), juce::Justification::centred, 1);
}

BandSplitterEditor::BandSplitterEditor (State& pluginState,
                                        dsp::band_splitter::Params& params,
                                        dsp::band_splitter::ExtraState& bandSplitterExtraState,
                                        const chowdsp::HostContextProvider& hcp,
                                        std::pair<gui::SpectrumAnalyserTask::Optional, gui::SpectrumAnalyserTask::Optional> spectrumAnalyserTasksLow,
                                        std::pair<gui::SpectrumAnalyserTask::Optional, gui::SpectrumAnalyserTask::Optional> spectrumAnalyserTasksMid,
                                        std::pair<gui::SpectrumAnalyserTask::Optional, gui::SpectrumAnalyserTask::Optional> spectrumAnalyserTasksHigh)
    : bandSplitterPlot (pluginState, params, bandSplitterExtraState, hcp, spectrumAnalyserTasksLow, spectrumAnalyserTasksMid, spectrumAnalyserTasksHigh),
      slopePicker (pluginState, *params.slope),
      extraState (bandSplitterExtraState),
      bandsButton (*params.threeBandOnOff, pluginState)
{
    addAndMakeVisible (bandSplitterPlot);
    addAndMakeVisible (slopePicker);
    addAndMakeVisible (bandsButton);

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
    bandsButton.setBounds (getWidth() - pad - dim, pad, dim, dim);
}
} // namespace gui::band_splitter
