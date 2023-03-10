#include "WaveshaperEditor.h"

namespace gui::waveshaper
{
WaveshaperEditor::WaveshaperEditor (State& pluginState, dsp::waveshaper::Params& wsParams)
    : plot (pluginState, wsParams),
      paramsView (pluginState, wsParams)
{
    addAndMakeVisible (plot);
    addAndMakeVisible (paramsView);
}

void WaveshaperEditor::resized()
{
    auto bounds = getLocalBounds();
    plot.setBounds (bounds.removeFromTop (proportionOfHeight (0.5f)));
    paramsView.setBounds (bounds);
}
} // namespace gui::waveshaper
