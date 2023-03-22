#pragma once

#include "dsp/Waveshaper/WaveshaperProcessor.h"

namespace gui::waveshaper
{
namespace spline = dsp::waveshaper::spline;

class WaveshaperDrawer : public juce::Component
{
public:
    explicit WaveshaperDrawer (dsp::waveshaper::ExtraState& wsExtraState);

    void paint (juce::Graphics& g) override;

    static chowdsp::WaveshaperPlotParams getPlotParams();
    juce::Path getDrawnPath (std::optional<chowdsp::WaveshaperPlotParams>&& params = {}) const;

private:
    void setSplinePoint (juce::Point<float> point);
    void visibilityChanged() override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseMove (const juce::MouseEvent& e) override;
    void mouseExit (const juce::MouseEvent& e) override;

    spline::SplineState& splineState;

    spline::SplinePoints points;
    std::optional<juce::Point<float>> mousePos;
    juce::Point<float> lastMouseDragPoint {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperDrawer)
};
} // namespace gui::waveshaper
