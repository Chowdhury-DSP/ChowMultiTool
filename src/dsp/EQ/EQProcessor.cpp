#include "EQProcessor.h"

namespace dsp::eq
{
EQProcessor::EQProcessor (const EQToolParams& eqParams, const ExtraState& es)
    : params (eqParams),
      extraState (es)
{
}

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

    preSpectrumAnalyserTask.prepareToPlay (spec.sampleRate, (int) spec.maximumBlockSize, (int) spec.numChannels);
    postSpectrumAnalyserTask.prepareToPlay (spec.sampleRate, (int) spec.maximumBlockSize, (int) spec.numChannels);
}

void EQProcessor::processBlock (const chowdsp::BufferView<float>& buffer)
{
    //pre-EQ
    if (extraState.isEditorOpen.load() && extraState.showPreSpectrum.get())
        preSpectrumAnalyserTask.processBlockInput (buffer.toAudioBuffer());

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

    //post-EQ
    if (extraState.isEditorOpen.load() && extraState.showPostSpectrum.get())
        postSpectrumAnalyserTask.processBlockInput (buffer.toAudioBuffer());
}

int EQProcessor::getLatencySamples() const
{
    return params.linearPhaseMode->get()
               ? linPhaseEQ.getLatencySamples()
               : 0;
}
} // namespace dsp::eq
