#include "EQProcessor.h"

// @TODO: handle latency from linear phase mode

namespace dsp::eq
{
void EQProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    linPhaseEQ.updatePrototypeEQParameters = [] (auto& pEQ, auto& eqParams)
    { pEQ.setParameters (eqParams); };

    eqBuffer.setMaxSize ((int) spec.numChannels, (int) spec.maximumBlockSize);

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
        chowdsp::copyToSIMDBuffer<float> (buffer, eqBuffer);
        eq.processBlock (eqBuffer);
        chowdsp::copyFromSIMDBuffer (eqBuffer, buffer);
    }
}
} // namespace dsp::eq
