#include "EQProcessor.h"

namespace dsp::eq
{
void EQProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    eqBuffer.setMaxSize ((int) spec.numChannels, (int) spec.maximumBlockSize);

    const auto&& eqParams = getEQParams();
    Params::EQParams::setEQParameters (eq, eqParams);
    eq.prepare (spec);


}

void EQProcessor::processBlock (const chowdsp::BufferView<float>& buffer)
{
    const auto&& eqParams = getEQParams();
    Params::EQParams::setEQParameters (eq, eqParams);
//    linPhaseEQ.setParameters (eqParams);

    if (params->linearPhaseMode->get())
    {

    }
    else
    {
        chowdsp::copyToSIMDBuffer<float> (buffer, eqBuffer);
        eq.processBlock (eqBuffer);
        chowdsp::copyFromSIMDBuffer (eqBuffer, buffer);
    }
}
}
