#pragma once

#include "dsp/Brickwall/BrickwallProcessor.h"
#include "state/PluginState.h"

namespace gui::brickwall
{
class BrickwallEditor : public juce::Component
{
public:
    BrickwallEditor (State& pluginState, dsp::brickwall::Params& params);

    void resized() override;

private:
    chowdsp::ParametersView paramsView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BrickwallEditor)
};
}
