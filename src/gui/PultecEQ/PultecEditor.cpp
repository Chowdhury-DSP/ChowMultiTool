#include "PultecEditor.h"

namespace gui::pultec
{
PultecEditor::PultecEditor (State& pluginState, dsp::pultec::Params& params)
    : paramsView (pluginState, params),
      plot (pluginState, params)
{
    addAndMakeVisible (paramsView);
    addAndMakeVisible (plot);
}

void PultecEditor::resized()
{
    auto bounds = getLocalBounds();
    plot.setBounds (bounds.removeFromTop (proportionOfHeight (0.5f)).reduced (5));
    paramsView.setBounds (bounds);
}
} // namespace gui::pultec
