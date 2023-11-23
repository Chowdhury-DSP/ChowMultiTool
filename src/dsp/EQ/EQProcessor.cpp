#include "EQProcessor.h"

namespace dsp::eq
{
void EQProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    linPhaseEQ.updatePrototypeEQParameters = [] (auto& pEQ, auto& eqParams)
    { pEQ.setParameters (eqParams); };

    doubleBuffer.setMaxSize ((int) spec.numChannels, (int) spec.maximumBlockSize);
    eqBuffer.setMaxSize (chowdsp::Math::ceiling_divide ((int) spec.numChannels, (int) EQFloat::size), (int) spec.maximumBlockSize);

    const auto&& eqParams = getEQParams();
    EQToolParams::EQParams::setEQParameters (eq, eqParams);
    eq.prepare (spec);
    linPhaseEQ.prepare (spec, getEQParams());
    //have this called only if bool is true
    //if (params.isOpen.load())
    spectrumAnalyserTask->prepareToPlay (spec.sampleRate, (int) spec.maximumBlockSize, (int) spec.numChannels);
}

void EQProcessor::processBlock (const chowdsp::BufferView<float>& buffer)
{
    auto numChannels = buffer.getNumChannels();
    auto numSamples = buffer.getNumSamples();

    //pre-EQ??

    juce::AudioBuffer<float> preEqAudioBuffer;
    preEqAudioBuffer.setSize (numChannels, numSamples);
    chowdsp::BufferMath::copyBufferData (buffer, preEqAudioBuffer);
    spectrumAnalyserTask->processBlockInput (preEqAudioBuffer);

    const auto&& eqParams = getEQParams();
    EQToolParams::EQParams::setEQParameters (eq, eqParams);
    linPhaseEQ.setParameters (eqParams);

    if (params.linearPhaseMode->get())
    {
        linPhaseEQ.processBlock (buffer);
    }
    else
    {
        doubleBuffer.setCurrentSize (buffer.getNumChannels(), buffer.getNumSamples());
        chowdsp::BufferMath::copyBufferData (buffer, doubleBuffer);

        chowdsp::copyToSIMDBuffer (doubleBuffer, eqBuffer);
        eq.processBlock (eqBuffer);
        chowdsp::copyFromSIMDBuffer (eqBuffer, doubleBuffer);

        chowdsp::BufferMath::copyBufferData (doubleBuffer, buffer);
    }

    //post-EQ??
    //    if (params.isOpen.load())
    //    {
    //        juce::AudioBuffer<float> postEqAudioBuffer;
    //        postEqAudioBuffer.setSize (numChannels, numSamples);
    //        chowdsp::BufferMath::copyBufferData (buffer, postEqAudioBuffer);
    //        //decide which buffer to send based on whether we are displaying pre or post spectrum analyser
    //        //have this called only if bool is true
    ////        spectrumAnalyserTask->processBlockInput (postEqAudioBuffer);
    //    }
}

int EQProcessor::getLatencySamples() const
{
    return params.linearPhaseMode->get()
               ? linPhaseEQ.getLatencySamples()
               : 0;
}
} // namespace dsp::eq
