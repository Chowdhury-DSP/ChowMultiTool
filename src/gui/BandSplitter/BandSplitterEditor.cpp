#include "BandSplitterEditor.h"

namespace gui::band_splitter
{
BandSplitterEditor::BandSplitterEditor (State& pluginState, dsp::band_splitter::Params& params)
    : slopeChoiceAttachment (*params.slope, pluginState, slopeChoiceBox),
      bandSplitterPlot (pluginState, params)
{
    addAndMakeVisible (slopeChoiceBox);
    addAndMakeVisible (bandSplitterPlot);
    addAndMakeVisible (slopePicker);
}

void BandSplitterEditor::paint (juce::Graphics& g)
{
    g.setGradientFill (juce::ColourGradient { juce::Colour { 0xFF211F1F },
                                              juce::Point { 0.0f, 0.0f },
                                              juce::Colour { 0xFF131111 },
                                              juce::Point { (float) getWidth() * 0.35f, (float) getHeight() * 0.5f },
                                              false });
    g.fillAll();
}

void BandSplitterEditor::resized()
{
    auto bounds = getLocalBounds();
    bandSplitterPlot.setBounds (bounds);
    slopePicker.setBounds (bounds.removeFromBottom (proportionOfHeight (0.05f)));
//    slopeChoiceBox.setBounds (bounds.reduced (20, 0));
}
} // namespace gui::band_splitter
