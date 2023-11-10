#include "EQHelpers.h"

EQHelpers::EQHelpers() = default;

void EQHelpers::prepareToPlay (double sampleRate, int samplesPerBlock, int numChannels)
{
    SpectrumAnalyserTask.prepare (sampleRate, samplesPerBlock, numChannels);
}

void EQHelpers::processBlockInput (juce::AudioBuffer<float>& buffer)
{
    SpectrumAnalyserTask.pushSamples (buffer);
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

void EQHelpers::SpectrumAnalyserBackgroundTask::prepareTask (double sampleRate, [[maybe_unused]] int samplesPerBlock, int& requestedBlockSize, int& waitMs)
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

void EQHelpers::SpectrumAnalyserBackgroundTask::resetTask()
{
    std::fill (fftMagsSmoothedDB.begin(), fftMagsSmoothedDB.end(), 0.0f);
    std::fill (magsPrevious.begin(), magsPrevious.end(), 0.0f);
}

void EQHelpers::SpectrumAnalyserBackgroundTask::runTask (const juce::AudioBuffer<float>& data)
{
    jassert (data.getNumSamples() == fftSize);

    scratchMonoBuffer.setCurrentSize (1, data.getNumSamples());
    chowdsp::BufferMath::sumToMono (data, scratchMonoBuffer);

    auto* scratchData = scratchMonoBuffer.getWritePointer (0);
    window->multiplyWithWindowingTable (scratchData, (size_t) fftSize);
    fft->performFrequencyOnlyForwardTransform (scratchData, true);

    juce::FloatVectorOperations::multiply (scratchData, 128.0f / (float) fftOutSize, fftOutSize);
    for (size_t i = 0; i < (size_t) fftOutSize; ++i)
        fftMagsUnsmoothedDB[i] = juce::Decibels::gainToDecibels (scratchData[i]);

    auto minMax = std::minmax_element (fftMagsUnsmoothedDB.begin(), fftMagsUnsmoothedDB.end());
    float dynamicRange = std::max (std::abs (*minMax.first), std::abs (*minMax.second));
    dynamicRange = std::clamp (dynamicRange, 20.0f, 40.0f);
    for (auto& dB : fftMagsUnsmoothedDB)
        dB = 20.0f * (dB / dynamicRange);

    const juce::CriticalSection::ScopedLockType lock { mutex };
    freqSmooth (fftMagsUnsmoothedDB.data(), fftMagsSmoothedDB.data(), fftOutSize, 1.0f / 128.0f);
    expSmooth (magsPrevious.data(), fftMagsSmoothedDB.data(), fftOutSize, 0.2f);
}

// notes for Rachel:
// - dynamic range trick is cool!
// - tweaked some things re: painting, frequency band smoothing, smoothing across time, and threading stuff
// - probably could use some slightly different coloring?
// - Could the spectrum task stuff live in the UI? That way the DSP can ignore it when the UI is closed.
// - namespaces and maybe new name/location for "EQHelpers" class
// - Add analyzer to other tools that have a frequency plot (could be a separate PR)
