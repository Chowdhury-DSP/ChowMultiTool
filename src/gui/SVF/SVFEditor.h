#pragma once

#include "SVFPlot.h"

namespace gui::svf
{
class SVFEditor : public juce::Component
{
public:
    SVFEditor (State& pluginState, dsp::svf::Params& svfParams, bool allowParamModulation);

    void resized() override;

private:
    chowdsp::ParametersView paramsView;
    SVFPlot plot;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SVFEditor)
};
} // namespace gui::svf
