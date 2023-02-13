#include "BrickwallEditor.h"

namespace gui::brickwall
{
BrickwallEditor::BrickwallEditor (State& pluginState, dsp::brickwall::Params& params)
    : paramsView (pluginState, params),
      plot (pluginState, params)
{
    addAndMakeVisible (paramsView);
    addAndMakeVisible (plot);
}

void BrickwallEditor::resized()
{
    auto bounds = getLocalBounds();
    plot.setBounds (bounds.removeFromTop (proportionOfHeight (0.5f)).reduced (5));
    paramsView.setBounds (bounds);
}
}
