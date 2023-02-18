#include "SVFEditor.h"

namespace gui::svf
{
SVFEditor::SVFEditor (State& pluginState, dsp::svf::Params& svfParams, bool allowParamModulation)
    : paramsView (pluginState, svfParams),
      plot (pluginState, svfParams, allowParamModulation)
{
    addAndMakeVisible (paramsView);
    addAndMakeVisible (plot);
}

void SVFEditor::resized()
{
    auto bounds = getLocalBounds();
    plot.setBounds (bounds.removeFromTop (proportionOfHeight (0.5f)).reduced (5));
    paramsView.setBounds (bounds);
}
} // namespace gui::svf
