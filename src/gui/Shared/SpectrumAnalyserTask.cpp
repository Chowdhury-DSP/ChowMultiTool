#include "SpectrumAnalyserTask.h"

namespace gui
{
static constexpr auto minusInfDB = -100.0f;

SpectrumAnalyserTask::SpectrumAnalyserTask() = default;

void SpectrumAnalyserTask::prepareToPlay (double sampleRate, int samplesPerBlock, int numChannels)
{
    spectrumAnalyserUITask.prepare (sampleRate, samplesPerBlock, numChannels);
}

void SpectrumAnalyserTask::reset()
{
    spectrumAnalyserUITask.resetTask();
}

void SpectrumAnalyserTask::processBlockInput (const juce::AudioBuffer<float>& buffer)
{
    spectrumAnalyserUITask.pushSamples (buffer);
}

static std::vector<float> getFFTFreqs (int N, float T)
{
    auto val = 0.5f / ((float) N * T);

    std::vector<float> results ((size_t) N, 0.0f);
    std::iota (results.begin(), results.end(), 0.0f);
    std::transform (results.begin(), results.end(), results.begin(), [val] (auto x)
                    { return x * val; });

    return results;
}

[[maybe_unused]] static void freqSmooth (const float* inData, float* outData, int numSamples, float smFactor = 1.0f / 24.0f)
{
    const auto s = smFactor > 1.0f ? smFactor : std::sqrt (std::exp2 (smFactor));
    for (int i = 0; i < numSamples; ++i)
    {
        auto i1 = std::max (int ((float) i / s), 0);
        auto i2 = std::min (int ((float) i * s) + 1, numSamples - 1);

        //        outData[i] = i2 > i1 ? std::accumulate (inData + i1, inData + i2, 0.0f) / std::pow (float (i2 - i1), 1.0f) : 0.0f;
        outData[i] = *std::max_element (inData + i1, inData + i2);
    }
}

//alternative to moving average
[[maybe_unused]] static void expSmooth (float* zData, float* outData, int numSamples, float alpha)
{
    for (int i = 0; i < numSamples; ++i)
    {
        outData[i] = alpha * outData[i] + (1 - alpha) * zData[i];
        zData[i] = outData[i];
    }
}

void SpectrumAnalyserTask::SpectrumAnalyserBackgroundTask::prepareTask (double sampleRate, [[maybe_unused]] int samplesPerBlock, int& requestedBlockSize, int& waitMs)
{
    static constexpr auto maxBinWidth = 6.0;
    fftSize = juce::nextPowerOfTwo (int (sampleRate / maxBinWidth));

    fft.emplace (chowdsp::Math::log2 (fftSize));
    window.emplace ((size_t) fftSize, juce::dsp::WindowingFunction<float>::WindowingMethod::triangular);

    fftDataSize = fftSize * 2;
    fftOutSize = fftSize / 2 + 1;

    requestedBlockSize = fftSize;
    waitMs = 10;

    scratchMonoBuffer.setMaxSize (1, fftDataSize);
    fftFreqs = getFFTFreqs (fftOutSize, 1.0f / (float) sampleRate);
    fftMagsUnsmoothedDB = std::vector<float> ((size_t) fftOutSize, 0.0f);
    fftMagsSmoothedDB = std::vector<float> ((size_t) fftOutSize, 0.0f);
    magsPrevious = std::vector<float> ((size_t) fftOutSize, 0.0f);
}

void SpectrumAnalyserTask::SpectrumAnalyserBackgroundTask::resetTask()
{
    const juce::CriticalSection::ScopedLockType lock { mutex };
    std::fill (fftMagsSmoothedDB.begin(), fftMagsSmoothedDB.end(), minusInfDB);
    std::fill (magsPrevious.begin(), magsPrevious.end(), minusInfDB);
}

void SpectrumAnalyserTask::SpectrumAnalyserBackgroundTask::runTask (const juce::AudioBuffer<float>& data)
{
    jassert (data.getNumSamples() == fftSize);

    scratchMonoBuffer.setCurrentSize (1, data.getNumSamples());
    chowdsp::BufferMath::sumToMono (data, scratchMonoBuffer);

    auto* scratchData = scratchMonoBuffer.getWritePointer (0);
    window->multiplyWithWindowingTable (scratchData, (size_t) fftSize);
    fft->performFrequencyOnlyForwardTransform (scratchData, true);

    juce::FloatVectorOperations::multiply (scratchData, 2.0f / (float) fftOutSize, fftOutSize);
    for (size_t i = 0; i < (size_t) fftOutSize; ++i)
        fftMagsUnsmoothedDB[i] = juce::Decibels::gainToDecibels (scratchData[i], minusInfDB);

    auto maxElement = std::max_element (fftMagsUnsmoothedDB.begin(), fftMagsUnsmoothedDB.end());
    if (*maxElement == minusInfDB)
    {
        std::fill (fftMagsUnsmoothedDB.begin(), fftMagsUnsmoothedDB.end(), minDB);
    }
    else
    {
        for (auto& dB : fftMagsUnsmoothedDB)
            dB = juce::jmap (dB,
                             minusInfDB,
                             std::max (*maxElement, maxDB - 6.0f),
                             minDB,
                             maxDB);
    }

    const juce::CriticalSection::ScopedLockType lock { mutex };
    freqSmooth (fftMagsUnsmoothedDB.data(), fftMagsSmoothedDB.data(), fftOutSize, 1.0f / 128.0f);
    expSmooth (magsPrevious.data(), fftMagsSmoothedDB.data(), fftOutSize, 0.15f);
}
} // namespace gui