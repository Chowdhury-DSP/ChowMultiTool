#include "EQEditor.h"

namespace gui::eq
{
EQEditor::EQEditor (State& pluginState, dsp::eq::Params& eqParams)
    : paramsView (pluginState, eqParams)
{
    addAndMakeVisible (paramsView);
}

void EQEditor::resized()
{
    paramsView.setBounds (getLocalBounds());
}
}
