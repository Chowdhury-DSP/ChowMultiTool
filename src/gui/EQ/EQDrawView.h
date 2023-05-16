#pragma once

#include "dsp/EQ/EQOptimiser.h"

namespace gui::eq
{
using Eigen::VectorXf;
using namespace LBFGSpp;

constexpr int maxNumDrawPoints = 600;
using EQPath = std::array<juce::Point<float>, maxNumDrawPoints>;
float getMagnitudeAtFrequency (const EQPath& eqPath, float frequencyHz, const chowdsp::SpectrumPlotParams& plotParams);

class EQDrawView : public juce::Component
{
public:
    explicit EQDrawView (const chowdsp::SpectrumPlotBase& spectrumPlot);

    void paint (juce::Graphics& g) override;
    std::array<float, dsp::eq::EQOptimiser::numPoints> getDrawnMagnitudeResponse();
    void triggerOptimiser (chowdsp::EQ::StandardEQParameters<dsp::eq::EQToolParams::numBands>& eqParameters);
    const dsp::eq::EQOptimiser& getOptimiser() const;

    chowdsp::Broadcaster<void()> onCompletedOptimisation;

private:
    void setEQPathPoint (juce::Point<float> point);
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseMove (const juce::MouseEvent& e) override;
    void mouseExit (const juce::MouseEvent& e) override;

    const chowdsp::SpectrumPlotBase& spectrumPlot;
    EQPath eqPath;
    dsp::eq::EQOptimiser optimiser;

    std::optional<juce::Point<float>> mousePos;
    juce::Point<float> lastMouseDragPoint {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQDrawView)
};
} // namespace gui::eq
