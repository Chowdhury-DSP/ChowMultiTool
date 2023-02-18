#include "EQProcessor.h"

// @TODO: handle latency from linear phase mode

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
}

void EQProcessor::processBlock (const chowdsp::BufferView<float>& buffer)
{
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
}
} // namespace dsp::eq
