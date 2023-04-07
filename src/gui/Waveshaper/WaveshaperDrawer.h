#pragma once

#include "dsp/Waveshaper/WaveshaperProcessor.h"

namespace gui::waveshaper
{
namespace spline = dsp::waveshaper::spline;

class WaveshaperDrawer : public juce::Component
{
public:
    WaveshaperDrawer (dsp::waveshaper::ExtraState& wsExtraState, juce::UndoManager& um);

    void paint (juce::Graphics& g) override;

    juce::Path getDrawnPath (std::optional<chowdsp::WaveshaperPlotParams>&& params = {}) const;

private:
    void visibilityChanged() override;
    void setSplinePoint (juce::Point<float> point);
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseMove (const juce::MouseEvent& e) override;
    void mouseExit (const juce::MouseEvent& e) override;

    spline::SplineState& freeDrawState;
    juce::UndoManager& um;

    spline::SplinePoints points;
    spline::SplinePoints prevPoints;
    std::optional<juce::Point<float>> mousePos;
    juce::Point<float> lastMouseDragPoint {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperDrawer)
};
} // namespace gui::waveshaper
