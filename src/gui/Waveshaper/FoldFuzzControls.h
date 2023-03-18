#pragma once

#include "dsp/Waveshaper/WaveshaperProcessor.h"
#include "state/PluginState.h"

namespace gui::waveshaper
{
class FoldFuzzControls : public juce::Component
{
public:
    FoldFuzzControls (State& pluginState, dsp::waveshaper::Params& wsParams);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    juce::Slider foldSlider, fuzzSlider;
    chowdsp::SliderAttachment foldAttach, fuzzAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FoldFuzzControls)
};
} // namespace gui::waveshaper
