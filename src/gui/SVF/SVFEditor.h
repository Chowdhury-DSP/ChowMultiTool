#pragma once

#include "dsp/SVF/SVFProcessor.h"
#include "state/PluginState.h"

namespace gui::svf
{
class SVFEditor : public juce::Component
{
public:
    SVFEditor (State& pluginState, dsp::svf::Params& svfParams);

    void resized() override;

private:
    chowdsp::ParametersView paramsView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SVFEditor)
};
}
