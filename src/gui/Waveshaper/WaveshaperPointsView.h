#pragma once

#include "dsp/Waveshaper/WaveshaperProcessor.h"

namespace gui::waveshaper
{
namespace spline = dsp::waveshaper::spline;
class WaveshaperPointsView : public juce::Component
{
public:
    WaveshaperPointsView (dsp::waveshaper::ExtraState& wsExtraState, juce::UndoManager& um);

    void paint (juce::Graphics& g) override;

    juce::Path getDrawnPath (std::optional<chowdsp::WaveshaperPlotParams>&& params = {}) const;

private:
    void visibilityChanged() override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseMove (const juce::MouseEvent& e) override;
    void mouseExit (const juce::MouseEvent& e) override;

    spline::VectorSplineState& splineState;
    juce::UndoManager& um;

    spline::VectorSplinePoints points;
    spline::VectorSplinePoints prevPoints;
    std::optional<juce::Point<float>> mousePos;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperPointsView)
};
} // namespace gui::waveshaper
