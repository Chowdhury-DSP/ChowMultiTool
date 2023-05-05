#pragma once

#include "dsp/SVF/SVFProcessor.h"
#include "state/PluginState.h"
#include "gui/Shared/VerticalSlider.h"

namespace gui::svf
{
class SVFParamControls : public juce::Component
{
public:
    SVFParamControls (State& pluginState, dsp::svf::Params& svfParams, const chowdsp::HostContextProvider& hcp);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void updateVisibilities();

    const dsp::svf::Params& svfParams;

    VerticalSlider modeSlider, qSlider, dampingSlider, driveSlider;
    chowdsp::SliderAttachment modeAttach, qAttach, dampingAttach, driveAttach;

    chowdsp::ScopedCallbackList callbacks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SVFParamControls)
};
} // namespace gui::svf
