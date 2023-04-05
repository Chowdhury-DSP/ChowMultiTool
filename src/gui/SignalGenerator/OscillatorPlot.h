#pragma once

#include <pch.h>

namespace gui::signal_gen
{
class OscillatorPlot : public chowdsp::SpectrumPlotBase
{
public:
    OscillatorPlot();

    void paint (juce::Graphics& g) override;

    void updatePlot (float gainParamDB);

    static constexpr auto analysisFs = 48000.0f;
#if JUCE_DEBUG
    static constexpr int fftSize = 1 << 14;
#else
    static constexpr int fftSize = 1 << 15;
#endif

    std::function<void (const chowdsp::BufferView<float>&)> plotUpdateCallback = nullptr;

private:
    chowdsp::StaticBuffer<float, 1, fftSize * 2> plotBuffer;
    juce::dsp::FFT fft { chowdsp::Math::log2 (fftSize) };

    std::array<float, (size_t) fftSize / 2 + 1> fftBinsSmoothDB {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscillatorPlot)
};
} // namespace gui::signal_gen