#pragma once

#include "BrickwallPlot.h"

namespace gui::brickwall
{
class BrickwallEditor : public juce::Component
{
public:
    BrickwallEditor (State& pluginState, dsp::brickwall::Params& params, const chowdsp::HostContextProvider& hcp);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    BrickwallPlot plot;
    std::unique_ptr<juce::Component> bottomBar;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BrickwallEditor)
};
} // namespace gui::brickwall
