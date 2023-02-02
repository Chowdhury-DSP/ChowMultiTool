#include "PultecEQProcessor.h"

namespace dsp::pultec
{
void PultecEQProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    for (auto& model : wdf)
        model.prepare ((float) spec.sampleRate);
}

void PultecEQProcessor::reset()
{
    for (auto& model : wdf)
        model.reset();
}

void PultecEQProcessor::processBlock (const chowdsp::BufferView<float>& buffer)
{
    const auto numChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();
    for (int ch = 0; ch < numChannels; ++ch)
    {
        wdf[ch].setParameters (params->trebleBoostParam->getCurrentValue(),
                               params->trebleBoostQParam->getCurrentValue(),
                               params->trebleBoostFreqParam->getIndex(),
                               params->trebleCutParam->getCurrentValue(),
                               params->trebleCutFreqParam->getIndex(),
                               params->bassBoostParam->getCurrentValue(),
                               params->bassCutParam->getCurrentValue(),
                               params->bassFreqParam->getIndex());

        auto* x = buffer.getWritePointer (ch);
        for (int n = 0; n < numSamples; ++n)
            x[n] = wdf[ch].process (x[n]);
    }

    chowdsp::BufferMath::applyGain (buffer, juce::Decibels::decibelsToGain (22.4f));
}
} // namespace dsp::pultec
