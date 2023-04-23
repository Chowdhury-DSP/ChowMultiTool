#pragma once

#include "BottomBar.h"
#include "SVFParamControls.h"
#include "SVFPlot.h"
#include "gui/Shared/IconButton.h"

namespace gui::svf
{
class SVFEditor : public juce::Component
{
public:
    SVFEditor (State& pluginState, dsp::svf::Params& svfParams, const chowdsp::HostContextProvider& hcp);

    void resized() override;
    void paint (juce::Graphics& g) override;

private:
    SVFPlot plot;
    SVFParamControls paramControls;
    std::unique_ptr<BottomBar> bottomBar;

    IconButton arpLimitButton;
    chowdsp::ButtonAttachment arpLimitAttach;
    chowdsp::ScopedCallback modeChangeCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SVFEditor)
};
} // namespace gui::svf
