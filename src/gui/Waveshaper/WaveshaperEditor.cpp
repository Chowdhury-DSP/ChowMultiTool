#include "WaveshaperEditor.h"

namespace gui::waveshaper
{
WaveshaperEditor::WaveshaperEditor (State& pluginState, dsp::waveshaper::Params& wsParams)
    : paramsView (pluginState, wsParams)
{
    addAndMakeVisible (paramsView);
}

void WaveshaperEditor::resized()
{
    paramsView.setBounds (getLocalBounds());
}
} // namespace gui::waveshaper
