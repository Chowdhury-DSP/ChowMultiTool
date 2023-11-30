#pragma once

#include "BrickwallPlot.h"
#include "gui/Shared/SpectrumAnalyser.h"

using optionalSpectrumBackgroundTask = std::optional<std::reference_wrapper<gui::SpectrumAnalyserTask::SpectrumAnalyserBackgroundTask>>;

namespace gui::brickwall
{
class BrickwallEditor : public juce::Component
{
public:
    BrickwallEditor (State& pluginState,
                     dsp::brickwall::Params& params,
                     dsp::brickwall::ExtraState& brickwallExtraState,
                     const chowdsp::HostContextProvider& hcp,
                     std::pair<optionalSpectrumBackgroundTask, optionalSpectrumBackgroundTask> spectrumAnalyserTasks);

    ~BrickwallEditor();

    void paint (juce::Graphics& g) override;
    void resized() override;

    void mouseDown (const juce::MouseEvent& event) override;

private:
    dsp::brickwall::ExtraState& extraState;
    chowdsp::ScopedCallbackList callbacks;

    BrickwallPlot plot;
    std::unique_ptr<juce::Component> bottomBar;
    SpectrumAnalyser spectrumAnalyser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BrickwallEditor)
};
} // namespace gui::brickwall
