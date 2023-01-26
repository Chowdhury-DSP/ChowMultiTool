#include "BandSplitterEditor.h"

namespace gui::band_splitter
{
BandSplitterEditor::BandSplitterEditor (State& pluginState, dsp::band_splitter::Params& params)
    : slopeChoiceAttachment (*params.slope, pluginState, slopeChoiceBox),
      bandSplitterPlot (pluginState, params)
{
    addAndMakeVisible (slopeChoiceBox);
    addAndMakeVisible (bandSplitterPlot);
}

void BandSplitterEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::dodgerblue);
}

void BandSplitterEditor::resized()
{
    auto bounds = getLocalBounds();
    bandSplitterPlot.setBounds (bounds.removeFromTop (proportionOfHeight (0.85f)).reduced (10));
    slopeChoiceBox.setBounds (bounds.reduced (20, 0));
}
} // namespace gui::band_splitter
