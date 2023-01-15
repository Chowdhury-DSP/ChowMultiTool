#include "BandSplitterEditor.h"

namespace gui::band_splitter
{
BandSplitterEditor::BandSplitterEditor (State& pluginState, dsp::band_splitter::Params& params)
    : paramsView (pluginState, params)
{
    addAndMakeVisible (paramsView);
}

void BandSplitterEditor::resized()
{
    paramsView.setBounds (getLocalBounds());
}
} // namespace gui::band_splitter
