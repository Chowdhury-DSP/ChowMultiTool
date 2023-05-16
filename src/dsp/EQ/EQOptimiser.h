//
// Created by Rachel Locke on 04/05/2023.
//
#pragma once

#include "EQProcessor.h"

namespace dsp::eq
{
using Eigen::VectorXf;
using namespace LBFGSpp;

class EQOptimiser
{
public:
    EQOptimiser();
    static constexpr size_t numPoints = 600;
    float operator() (const VectorXf& x, VectorXf& grad, bool is_top_level = true);
    void runOptimiser (std::array<float, numPoints>&& desiredMagnitudeResponse);
    void updateEQParameters (chowdsp::EQ::StandardEQParameters<EQToolParams::numBands>& eqParameters) const;

    static constexpr std::array<float, numPoints> freqs = []
    {
        std::array<float, numPoints> freqsArray {};
        float start = 20.0;
        float stop = 20000.0;
        auto factor = float (gcem::pow (stop / start, 1.0f / (numPoints - 1)));
        for (size_t i = 0; i < numPoints; i++)
        {
            freqsArray[i] = start * float (gcem::pow (factor, i));
        }
        return freqsArray;
    }();

    std::atomic_int iterationCount { 0 };

private:
    VectorXf optParams = VectorXf::Constant (24, 0.0f);
    std::array<float, numPoints> desiredMagResponse {};
    LBFGSBParam<float> param;
    LBFGSBSolver<float> solver { param };
    static constexpr float stepSize = 0.0001f;
    static constexpr int maxIter = 250;
};
} // namespace dsp::eq
