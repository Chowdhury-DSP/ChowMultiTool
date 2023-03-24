#pragma once

#include "state/PluginState.h"
#include "dsp/EQ/EQProcessor.h"
#include "EQBandBox.h"

namespace gui::eq
{
class BottomBar : public juce::Component
{
public:
    BottomBar (State& pluginState, dsp::eq::EQToolParams& params);

    void resized() override;
    void paint (juce::Graphics& g) override;

private:
    std::array<EQBandBox, dsp::eq::EQToolParams::numBands> boxes;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BottomBar)
};
}
