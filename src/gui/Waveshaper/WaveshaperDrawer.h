#pragma once

#include "dsp/Waveshaper/WaveshaperProcessor.h"

namespace gui::waveshaper
{
class WaveshaperDrawer : public juce::Component
{
public:
    explicit WaveshaperDrawer (dsp::waveshaper::ExtraState& wsExtraState);

    void paint (juce::Graphics& g) override;

    static chowdsp::WaveshaperPlotParams getPlotParams();
    juce::Path getDrawnPath (std::optional<chowdsp::WaveshaperPlotParams>&& params = {}) const;

private:
    void visibilityChanged() override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseMove (const juce::MouseEvent& e) override;
    void mouseExit (const juce::MouseEvent& e) override;

    dsp::waveshaper::SplineState& splineState;

    std::vector<juce::Point<float>> points;
    std::optional<juce::Point<float>> mousePos;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperDrawer)
};
}
