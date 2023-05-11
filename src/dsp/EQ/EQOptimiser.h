//
// Created by Rachel Locke on 04/05/2023.
//
#pragma once
#include "EQProcessor.h"
#include <pch.h>

using Eigen::VectorXf;

class EQOptimiser
{
public:
    static constexpr size_t numPoints = 600;
    float operator() (const VectorXf& x, VectorXf& grad, bool is_top_level = true);
    void runOptimiser (std::array<float, numPoints>&& desiredMagnitudeResponse);
    const std::array<float, numPoints> freqs = [] {
        std::array<float, numPoints> freqsArray{};
        float start = 20.0;
        float stop = 20000.0;
        auto factor = float(std::pow(stop / start, 1.0f / (numPoints - 1)));
        for (size_t i = 0; i < numPoints; i++) {
            freqsArray[i] = start * float(std::pow(factor, i));
        }
        return freqsArray;
    }();
    VectorXf getOptParamValues();

private:
    VectorXf optParams = VectorXf::Constant(24, 0.0f);
    std::array<float, numPoints> desiredMagResponse;
    const float stepSize = 0.0001f;
};

