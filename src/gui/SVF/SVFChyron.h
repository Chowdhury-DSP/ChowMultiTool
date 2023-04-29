#pragma once

#include "dsp/SVF/SVFProcessor.h"
#include "gui/Shared/Fonts.h"
#include "gui/Shared/TextSlider.h"

namespace gui::svf
{
class SVFChyron : public juce::Component
{
public:
    SVFChyron (chowdsp::PluginState& pluginState, dsp::svf::Params& params, const chowdsp::HostContextProvider& hcp);

    void resized() override;
    void paint (juce::Graphics& g) override;

    void keytrackParamChanged (bool keytrackModeOn);

private:
    TextSlider freqSlider;
    TextSlider keytrackOffsetSlider;

    SharedFonts fonts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SVFChyron)
};
}
