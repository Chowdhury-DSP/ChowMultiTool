#pragma once

#include "AnalogEQPlot.h"

namespace gui::analog_eq
{
class AnalogEQEditor : public juce::Component
{
public:
    AnalogEQEditor (State& pluginState, dsp::analog_eq::Params& params);

    void resized() override;

private:
    chowdsp::ParametersView paramsView;
    AnalogEQPlot plot;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalogEQEditor)
};
} // namespace gui::pultec
