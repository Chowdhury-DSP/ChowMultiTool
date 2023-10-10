#include "EQHelpers.h"


EQHelpers::EQHelpers()
{

}

void EQHelpers::prepareToPlay (double sampleRate, int samplesPerBlock, int numChannels)
{
    SpectrumAnalyserTask.prepare(sampleRate, samplesPerBlock, numChannels);
}

void EQHelpers::processBlockInput (juce::AudioBuffer<float>& buffer)
{
    SpectrumAnalyserTask.pushSamples(buffer);
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
    const auto s = smFactor > 1.0f ? smFactor : std::sqrt (std::pow (2.0f, smFactor));
    for (int i = 0; i < numSamples; ++i)
    {
        auto i1 = std::max (int ((float) i / s), 0);
        auto i2 = std::min (int ((float) i * s) + 1, numSamples - 1);

        outData[i] = i2 > i1 ? std::accumulate (inData + i1, inData + i2, 0.0f) / float (i2 - i1) : 0.0f;
    }
}

void EQHelpers::SpectrumAnalyserBackgroundTask::prepareTask(double sampleRate, [[maybe_unused]] int samplesPerBlock, int& requestedBlockSize, int& waitMs)
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
    fftMagsUnsmoothedDB = std::vector<float> ((size_t) fftOutSize, -100.0f);
    fftMagsSmoothedDB = std::vector<float> ((size_t) fftOutSize, -100.0f);
}
void EQHelpers::SpectrumAnalyserBackgroundTask::resetTask()
{
    std::fill (fftMagsSmoothedDB.begin(), fftMagsSmoothedDB.end(), -100.0f);
}
void EQHelpers::SpectrumAnalyserBackgroundTask::runTask(const juce::AudioBuffer<float> &data)
{
    jassert (data.getNumSamples() == fftSize);

    scratchMonoBuffer.setCurrentSize (1, data.getNumSamples());
    chowdsp::BufferMath::sumToMono (data, scratchMonoBuffer);

    auto* scratchData = scratchMonoBuffer.getWritePointer (0);
    window->multiplyWithWindowingTable (scratchData, (size_t) fftSize);
    fft->performFrequencyOnlyForwardTransform (scratchData, true);

    juce::FloatVectorOperations::multiply (scratchData, 16.0f / (float) fftSize, fftOutSize);
    for (size_t i = 0; i < (size_t) fftOutSize; ++i)
        fftMagsUnsmoothedDB[i] = 0.05f * juce::Decibels::gainToDecibels (scratchData[i]) + 0.95f * fftMagsUnsmoothedDB[i];

    const juce::CriticalSection::ScopedLockType lock { mutex };
    freqSmooth (fftMagsUnsmoothedDB.data(), fftMagsSmoothedDB.data(), fftOutSize, 1.0f / 64.0f);
}