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
    SVFEditor (State& pluginState,
               dsp::svf::Params& svfParams,
               dsp::svf::ExtraState& extraState,
               const chowdsp::HostContextProvider& hcp,
               SpectrumAnalyserTask::PrePostPair spectrumAnalyserTasks);

    void resized() override;
    void paint (juce::Graphics& g) override;

private:
    SVFPlot plot;
    SVFParamControls paramControls;
    std::unique_ptr<BottomBar> bottomBar;

    std::unique_ptr<IconButton> keytrackButton;
    chowdsp::ButtonAttachment keytrackAttach;
    IconButton arpLimitButton;
    chowdsp::ButtonAttachment arpLimitAttach;
    chowdsp::ScopedCallback modeChangeCallback;

    juce::TooltipWindow tooltips { this, 300 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SVFEditor)
};
} // namespace gui::svf
