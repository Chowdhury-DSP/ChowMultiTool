#include "BrickwallEditor.h"

namespace gui::brickwall
{
BrickwallEditor::BrickwallEditor (State& pluginState, dsp::brickwall::Params& params)
    : paramsView (pluginState, params)
{
    addAndMakeVisible (paramsView);
}

void BrickwallEditor::resized()
{
    paramsView.setBounds (getLocalBounds());
}
}
