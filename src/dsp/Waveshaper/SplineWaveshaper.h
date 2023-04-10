#pragma once

#include "SplineHelpers.h"

namespace dsp::waveshaper::spline
{
// Fixed size spline:
using SplinePoints = std::array<juce::Point<float>, maxNumDrawPoints>;
using Spline = std::array<SplineSection, maxNumDrawPoints - 1>;
using SplineADAA = std::pair<Spline, std::array<SplineADAASection, maxNumDrawPoints - 1>>;

struct DefaultSplineCreator
{
    static SplinePoints call();
};

Spline createSpline (const SplinePoints& points);
double evaluateSpline (const Spline& spline, double x);

using SplineState = SplinePointsState<SplinePoints>;
using UndoableSplineSet = UndoableSplinePointsSet<SplinePoints>;

// Variable size spline:
using VectorSplinePoints = std::vector<juce::Point<float>>;
using VectorSpline = std::vector<SplineSection>;
using VectorSplineADAA = std::pair<VectorSpline, std::vector<SplineADAASection>>;

struct DefaultVectorSplineCreator
{
    static VectorSplinePoints call();
};

VectorSpline createSpline (const VectorSplinePoints& points);
double evaluateSpline (const VectorSpline& spline, double x);

using VectorSplineState = SplinePointsState<VectorSplinePoints>;
using UndoableVectorSplineSet = UndoableSplinePointsSet<VectorSplinePoints>;

template <typename SplinePointsType, typename SplineADAAType>
class SplineWaveshaper
{
public:
    explicit SplineWaveshaper (SplinePointsState<SplinePointsType>& splineState);

    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();

    void processBlock (const chowdsp::BufferView<double>& buffer) noexcept;

private:
    SplinePointsState<SplinePointsType>& splineState;

    std::vector<double> x1;
    chowdsp::FirstOrderHPF<double> dcBlocker;

    chowdsp::UIToAudioPipeline<SplineADAAType> splineUIToAudioPipeline;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SplineWaveshaper)
};
} // namespace dsp::waveshaper::spline
