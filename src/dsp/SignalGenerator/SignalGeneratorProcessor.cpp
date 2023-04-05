#include "SignalGeneratorProcessor.h"

namespace dsp::signal_gen
{
static constexpr int oversampleRatio = 2;

void SignalGeneratorProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    chowdsp::TupleHelpers::forEachInTuple ([&spec] (auto& oscillator, size_t)
                                           { oscillator.prepare ({ 2.0 * spec.sampleRate, 2 * spec.maximumBlockSize, 1 }); },
                                           oscillators);

    gain.setGainDecibels (params.gain->getCurrentValue());
    gain.setRampDurationSeconds (0.05);
    gain.prepare ({ spec.sampleRate, spec.maximumBlockSize, 1 });

    freqParamSmoothed.prepare (oversampleRatio * spec.sampleRate, oversampleRatio * (int) spec.maximumBlockSize);
    freqParamSmoothed.setRampLength (0.05);

    downsampler.prepare ({ oversampleRatio * spec.sampleRate, oversampleRatio * spec.maximumBlockSize, 1 }, oversampleRatio);
    upsampledBuffer.setMaxSize (1, oversampleRatio * (int) spec.maximumBlockSize);

    nyquistHz = (float) spec.sampleRate / 2.0f;
}

void SignalGeneratorProcessor::reset()
{
    freqParamSmoothed.reset (juce::jmin (params.frequency->getCurrentValue(), nyquistHz));

    gain.setGainDecibels (params.gain->getCurrentValue());
    gain.reset();

    downsampler.reset();

    chowdsp::TupleHelpers::visit_at (oscillators,
                                     static_cast<size_t> (params.oscillatorChoice->get()),
                                     [] (auto& oscillator)
                                     {
                                         oscillator.reset();
                                     });
}

void SignalGeneratorProcessor::processBlock (const chowdsp::BufferView<float>& buffer)
{
    const auto numChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();

    // Generate signal up-sampled
    upsampledBuffer.setCurrentSize (1, oversampleRatio * numSamples);
    upsampledBuffer.clear();
    freqParamSmoothed.process (juce::jmin (params.frequency->getCurrentValue(), nyquistHz), oversampleRatio * numSamples);
    chowdsp::TupleHelpers::visit_at (oscillators,
                                     static_cast<size_t> (params.oscillatorChoice->getIndex()),
                                     [this] (auto& oscillator)
                                     {
                                         if (freqParamSmoothed.isSmoothing())
                                         {
                                             const auto* freqHzData = freqParamSmoothed.getSmoothedBuffer();
                                             for (auto [n, x] : chowdsp::enumerate (upsampledBuffer.getWriteSpan (0)))
                                             {
                                                 oscillator.setFrequency (freqHzData[n]);
                                                 x = oscillator.processSample();
                                             }
                                         }
                                         else
                                         {
                                             oscillator.setFrequency (freqParamSmoothed.getCurrentValue());
                                             oscillator.processBlock (upsampledBuffer);
                                         }
                                     });

    // downsample to DAW sample rate
    auto downsampledBuffer = downsampler.process (upsampledBuffer);
    gain.setGainDecibels (params.gain->getCurrentValue());
    gain.process (downsampledBuffer);

    // split from mono to multi-channel
    for (int ch = 0; ch < numChannels; ++ch)
        chowdsp::BufferMath::copyBufferChannels (downsampledBuffer, buffer, 0, ch);
}
} // namespace dsp::signal_gen
