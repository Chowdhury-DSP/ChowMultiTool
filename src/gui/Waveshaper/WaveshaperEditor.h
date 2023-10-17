#pragma once

#include "FoldFuzzControls.h"
#include "WaveshaperPlot.h"
#include "gui/Shared/IconButton.h"

namespace gui::waveshaper
{
class WaveshaperEditor : public juce::Component
{
public:
    WaveshaperEditor (State& pluginState, dsp::waveshaper::Params& wsParams, const chowdsp::HostContextProvider& hcp);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    dsp::waveshaper::Params& params;

    WaveshaperPlot plot;
    std::unique_ptr<juce::Component> bottomBar;
    FoldFuzzControls foldFuzzControls;

    IconButton clipGuardButton;
    chowdsp::ButtonAttachment clipGuardAttach;

    IconButton freeDrawButton;
    IconButton mathButton;
    IconButton pointsButton;

    chowdsp::ScopedCallbackList callbacks;

    juce::TooltipWindow tooltips { this, 300 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperEditor)
};
} // namespace gui::waveshaper
