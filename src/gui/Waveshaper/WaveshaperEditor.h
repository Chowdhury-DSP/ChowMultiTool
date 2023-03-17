#pragma once

#include "FoldFuzzControls.h"
#include "WaveshaperPlot.h"

namespace gui::waveshaper
{
class WaveshaperEditor : public juce::Component
{
public:
    WaveshaperEditor (State& pluginState, dsp::waveshaper::Params& wsParams);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    dsp::waveshaper::Params& params;

    WaveshaperPlot plot;
    std::unique_ptr<juce::Component> bottomBar;
    FoldFuzzControls foldFuzzControls;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperEditor)
};
} // namespace gui::waveshaper
