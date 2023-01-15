#pragma once

#include "dsp/PultecEQ/PultecEQProcessor.h"
#include "state/PluginState.h"

namespace gui::pultec
{
class PultecEditor : public juce::Component
{
public:
    PultecEditor (State& pluginState, dsp::pultec::Params& params);

    void resized() override;

private:
    chowdsp::ParametersView paramsView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PultecEditor)
};
} // namespace gui::pultec
