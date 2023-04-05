#include "AnalogEQEditor.h"

namespace gui::analog_eq
{
AnalogEQEditor::AnalogEQEditor (State& pluginState, dsp::analog_eq::Params& params)
    : paramsView (pluginState, params),
      plot (pluginState, params)
{
    addAndMakeVisible (paramsView);
    addAndMakeVisible (plot);
}

void AnalogEQEditor::resized()
{
    auto bounds = getLocalBounds();
    plot.setBounds (bounds.removeFromTop (proportionOfHeight (0.5f)).reduced (5));
    paramsView.setBounds (bounds);
}
} // namespace gui::analog_eq
