#include "PultecEditor.h"

namespace gui::pultec
{
PultecEditor::PultecEditor (State& pluginState, dsp::pultec::Params& params)
    : paramsView (pluginState, params)
{
    addAndMakeVisible (paramsView);
}

void PultecEditor::resized()
{
    paramsView.setBounds (getLocalBounds());
}
} // namespace gui::pultec
