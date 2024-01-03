#pragma once

#include "BrickwallPlot.h"
#include "gui/Shared/SpectrumAnalyser.h"

namespace gui::brickwall
{
class BrickwallEditor : public juce::Component
{
public:
    BrickwallEditor (State& pluginState,
                     dsp::brickwall::Params& params,
                     dsp::brickwall::ExtraState& brickwallExtraState,
                     const chowdsp::HostContextProvider& hcp,
                     std::pair<gui::SpectrumAnalyserTask::OptionalBackgroundTask, gui::SpectrumAnalyserTask::OptionalBackgroundTask> spectrumAnalyserTasks);

    ~BrickwallEditor();

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    chowdsp::ScopedCallbackList callbacks;

    BrickwallPlot plot;
    std::unique_ptr<juce::Component> bottomBar;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BrickwallEditor)
};
} // namespace gui::brickwall
