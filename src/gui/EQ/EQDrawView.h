#pragma once
#include <pch.h>
#include "dsp/EQ/EQOptimiser.h"

using Eigen::VectorXf;
using namespace LBFGSpp;

namespace gui::eq
{
constexpr int maxNumDrawPoints = 600;
using EQPath = std::array<juce::Point<float>, maxNumDrawPoints>;
float getMagnitudeAtFrequency (const EQPath& eqPath, float frequencyHz, const chowdsp::SpectrumPlotParams& plotParams);

class EQDrawView : public juce::Component
{
public:
    explicit EQDrawView (const chowdsp::SpectrumPlotBase& spectrumPlot);

    void paint (juce::Graphics& g) override;
    std::array<float, EQOptimiser::numPoints> getDrawnMagnitudeResponse();
    void triggerOptimiser();

private:
    void setEQPathPoint (juce::Point<float> point);
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseMove (const juce::MouseEvent& e) override;
    void mouseExit (const juce::MouseEvent& e) override;

    const chowdsp::SpectrumPlotBase& spectrumPlot;
    EQPath eqPath;
    EQOptimiser optimiser;

    std::optional<juce::Point<float>> mousePos;
    juce::Point<float> lastMouseDragPoint {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQDrawView)
};
}