#pragma once

#include "dsp/Waveshaper/WaveshaperProcessor.h"

namespace gui::waveshaper
{
namespace spline = dsp::waveshaper::spline;

class WaveshaperMathView : public juce::Component
{
public:
    WaveshaperMathView (dsp::waveshaper::ExtraState& wsExtraState, juce::UndoManager& um);

    void paint (juce::Graphics& g) override;
    void resized() override;

    juce::Path getDrawnPath (std::optional<chowdsp::WaveshaperPlotParams>&& params = {}) const;

private:
    void attemptToLoadNewMathShaper();
    void visibilityChanged() override;

    spline::SplineState& mathState;
    juce::UndoManager& um;
    spline::SplinePoints prevPoints;

    juce::TextEditor mathBox;
    juce::TextButton enterButton { "ENTER" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperMathView)
};
} // namespace gui::waveshaper
