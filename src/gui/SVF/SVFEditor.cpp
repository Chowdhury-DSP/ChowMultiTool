#include "SVFEditor.h"

namespace gui::svf
{
    SVFEditor::SVFEditor (State& pluginState, dsp::svf::Params& svfParams)
        : paramsView (pluginState, svfParams)
    {
        addAndMakeVisible (paramsView);
    }

    void SVFEditor::resized()
    {
        paramsView.setBounds (getLocalBounds());
    }
}
