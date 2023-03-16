#include "BandSplitterEditor.h"
#include "BandSplitterColours.h"

namespace gui::band_splitter
{
BandSplitterEditor::BandSplitterEditor (State& pluginState, dsp::band_splitter::Params& params)
    : bandSplitterPlot (pluginState, params),
      slopePicker (pluginState)
{
    addAndMakeVisible (bandSplitterPlot);
    addAndMakeVisible (slopePicker);
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
}
} // namespace gui::band_splitter
