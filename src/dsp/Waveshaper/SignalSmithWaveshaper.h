#pragma once

#include <pch.h>

namespace dsp::waveshaper
{
// Inspired by a waveshaping function shared on the Audio Programmer Discord by Signalsmith
// Reference: https://www.desmos.com/calculator/uoqx3doi1z
class SignalSmithWaveshaper
{
public:
    SignalSmithWaveshaper() = default;

    void prepare (double sample_rate, int block_size, int numChannels);
    void reset();

    void processBlock (const chowdsp::BufferView<xsimd::batch<double>>& buffer, float k_param, float M_param) noexcept;

private:
    // params
    chowdsp::SmoothedBufferValue<double> k_smooth;
    chowdsp::SmoothedBufferValue<double, juce::ValueSmoothingTypes::Multiplicative> M_smooth;

    // state
    std::vector<xsimd::batch<double>> x1;
    std::vector<xsimd::batch<double>> x2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SignalSmithWaveshaper)
};
}
