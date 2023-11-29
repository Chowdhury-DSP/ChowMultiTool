#pragma once

#include <pch.h>

namespace gui
{
class SpectrumAnalyserTask
{
public:
    SpectrumAnalyserTask();
    void prepareToPlay (double sampleRate, int samplesPerBlock, int numChannels);
    void processBlockInput (const juce::AudioBuffer<float>& buffer);

    struct SpectrumAnalyserBackgroundTask : chowdsp::TimeSliceAudioUIBackgroundTask
    {
        SpectrumAnalyserBackgroundTask() : chowdsp::TimeSliceAudioUIBackgroundTask ("Spectrum Analyser Background Task") {}

        void prepareTask (double sampleRate, [[maybe_unused]] int samplesPerBlock, int& requestedBlockSize, int& waitMs) override;
        void resetTask() override;
        void runTask (const juce::AudioBuffer<float>& data) override;

        juce::CriticalSection mutex {};
        std::vector<float> fftFreqs {};
        std::vector<float> fftMagsSmoothedDB {};

    private:
        std::optional<juce::dsp::FFT> fft {};
        std::optional<juce::dsp::WindowingFunction<float>> window {};

        int fftSize = 0;
        int fftDataSize = 0;
        int fftOutSize = 0;

        chowdsp::Buffer<float> scratchMonoBuffer {};
        std::vector<float> fftMagsUnsmoothedDB {};
        std::vector<float> magsPrevious {};
    } SpectrumAnalyserUITask;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumAnalyserTask)
};
} // namespace gui