#pragma once

#include "dsp/EQ/EQProcessor.h"
#include "state/PluginState.h"

namespace gui::eq
{
class EQEditor : public juce::Component
{
public:
    EQEditor (State& pluginState, dsp::eq::Params& eqParams);

    void resized() override;

private:
    chowdsp::ParametersView <State, dsp::eq::Params> paramsView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQEditor)
};
} // namespace gui::eq
