#include "OscillatorPlot.h"

namespace gui::signal_gen
{
template <int N>
constexpr std::array<float, N> getFFTFreqs (float fs)
{
    auto val = 0.5f * fs / (float) N;

    std::array<float, N> results {};
    std::iota (results.begin(), results.end(), 0.0f);
    std::transform (results.begin(), results.end(), results.begin(), [val] (auto x)
                    { return x * val; });

    return results;
}

[[maybe_unused]] static void freqSmooth (const float* inData, float* outData, int numSamples) //, [[maybe_unused]] float smFactor = 1.0f / 24.0f)
{
    std::copy (inData, inData + numSamples, outData);

    //    const auto s = smFactor > 1.0f ? smFactor : std::sqrt (std::pow (2.0f, smFactor));
    //    for (int i = 0; i < numSamples; ++i)
    //    {
    //        auto i1 = std::max (int ((float) i / s), 0);
    //        auto i2 = std::min (int ((float) i * s) + 1, numSamples - 1);
    //
    //        outData[i] = i2 > i1 ? std::accumulate (inData + i1, inData + i2, 0.0f) / float (i2 - i1) : 0.0f;
    //    }
}

OscillatorPlot::OscillatorPlot()
    : chowdsp::SpectrumPlotBase (chowdsp::SpectrumPlotParams {
        .minFrequencyHz = 10.0f,
        .maxFrequencyHz = 24000.0f,
        .minMagnitudeDB = -66.0f,
        .maxMagnitudeDB = 30.0f,
    })
{
    plotBuffer.setMaxSize (1, fft.getSize() * 2); // over-allocate by 2x so that FFT has extra space
}

void OscillatorPlot::paint (juce::Graphics& g)
{
    g.setColour (juce::Colours::white);
    drawFrequencyLines (g, { 100.0f, 1000.0f, 10'000.0f });
    drawMagnitudeLines (g, { -60.0f, -48.0f, -36.0f, -24.0f, -12.0f, 0.0f, 12.0f, 24.0f });

    juce::Path plotPath {};
    static constexpr auto fftFreqs = getFFTFreqs<(size_t) fftSize / 2 + 1> (analysisFs);

    bool started = false;
    for (const auto [freq, binDB] : chowdsp::zip (fftFreqs, fftBinsSmoothDB))
    {
        if (freq < params.minFrequencyHz || freq > params.maxFrequencyHz)
            continue;

        auto xDraw = getXCoordinateForFrequency (freq);
        auto yDraw = getYCoordinateForDecibels (binDB);

        if (! started)
        {
            plotPath.startNewSubPath (xDraw, yDraw);
            started = true;
        }
        else
        {
            plotPath.lineTo (xDraw, yDraw);
        }
    }

    //    for (size_t i = 0; i < (size_t) fftSize / 2 + 1; ++i)
    //    {
    //        if (fftFreqs[i] < params.minFrequencyHz / 2.0f || fftFreqs[i] > params.maxFrequencyHz * 1.01f)
    //            continue;
    //
    //        auto xDraw = getXCoordinateForFrequency (fftFreqs[i]);
    //        auto yDraw = getYCoordinateForDecibels (fftBinsSmoothDB[i]);
    //
    //        if (! started)
    //        {
    //            plotPath.startNewSubPath (xDraw, yDraw);
    //            started = true;
    //        }
    //        else
    //        {
    //            plotPath.lineTo (xDraw, yDraw);
    //        }
    //    }

    plotPath.lineTo (juce::Point { getWidth(), getHeight() }.toFloat());

    g.setColour (juce::Colours::red);
    g.strokePath (plotPath, juce::PathStrokeType { 2.0f });
}

void OscillatorPlot::updatePlot()
{
    plotBuffer.clear();
    plotUpdateCallback (chowdsp::BufferView<float> { plotBuffer, 0, fft.getSize() });

    fft.performFrequencyOnlyForwardTransform (plotBuffer.getWritePointer (0), true);

    chowdsp::BufferMath::applyFunctionSIMD (
        plotBuffer,
        [] (const auto& x)
        {
            return chowdsp::SIMDUtils::gainToDecibels (x * x) - 80.0f;
        });
    freqSmooth (plotBuffer.getReadPointer (0), fftBinsSmoothDB.data(), (int) fftBinsSmoothDB.size()); //, 1.0f / 4.0f);

    repaint();
}
} // namespace gui::signal_gen
