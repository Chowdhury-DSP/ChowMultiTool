#include "SignalGeneratorProcessor.h"

namespace dsp::signal_gen
{
void SignalGeneratorProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    const auto monoSpec = juce::dsp::ProcessSpec { spec.sampleRate, spec.maximumBlockSize, 1 };
    chowdsp::TupleHelpers::forEachInTuple ([&monoSpec] (auto& oscillator, size_t)
                                           { oscillator.prepare (monoSpec); },
                                           oscillators);

    gain.setGainDecibels (params.gain->getCurrentValue());
    gain.setRampDurationSeconds (0.05);
    gain.prepare (monoSpec);

    freqParamSmoothed.prepare (spec.sampleRate, (int) spec.maximumBlockSize);
    freqParamSmoothed.setRampLength (0.05);

    nyquistHz = (float) spec.sampleRate / 2.0f;
}

void SignalGeneratorProcessor::reset()
{
    freqParamSmoothed.reset (juce::jmin (params.frequency->getCurrentValue(), nyquistHz));

    gain.setGainDecibels (params.gain->getCurrentValue());
    gain.reset();

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

    // do processing in mono
    auto monoBuffer = chowdsp::BufferView<float> { buffer, 0, -1, 0, 1 };
    monoBuffer.clear();

    freqParamSmoothed.process (juce::jmin (params.frequency->getCurrentValue(), nyquistHz), numSamples);
    chowdsp::TupleHelpers::visit_at (oscillators,
                                     static_cast<size_t> (params.oscillatorChoice->getIndex()),
                                     [this, &monoBuffer, numSamples] (auto& oscillator)
                                     {
                                         if (freqParamSmoothed.isSmoothing())
                                         {
                                             const auto* freqHzData = freqParamSmoothed.getSmoothedBuffer();
                                             auto* data = monoBuffer.getWritePointer (0);
                                             for (int n = 0; n < numSamples; ++n)
                                             {
                                                 oscillator.setFrequency (freqHzData[n]);
                                                 data[n] = oscillator.processSample();
                                             }
                                         }
                                         else
                                         {
                                             oscillator.setFrequency (freqParamSmoothed.getCurrentValue());
                                             oscillator.processBlock (monoBuffer);
                                         }
                                     });

    gain.setGainDecibels (params.gain->getCurrentValue());
    gain.process (monoBuffer);

    // back from mono to multi-channel
    for (int ch = 1; ch < numChannels; ++ch)
        chowdsp::BufferMath::copyBufferChannels (buffer, buffer, 0, ch);
}
} // namespace dsp::signal_gen
