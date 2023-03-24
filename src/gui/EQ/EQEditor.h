#pragma once

#include "EQPlot.h"

// @TODO:
// - colours
// - some notion of "selected" band
// - "chiron" in the bottom corner to show params for selected band
// - Icon button for linear phase

namespace gui::eq
{
class EQEditor : public juce::Component
{
public:
    EQEditor (State& pluginState, dsp::eq::EQToolParams& eqParams);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    EQPlot plot;
    std::unique_ptr<juce::Component> bottomBar;
    chowdsp::ParametersView paramsView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQEditor)
};
} // namespace gui::eq
