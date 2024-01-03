#include "AnalogEQProcessor.h"

namespace dsp::analog_eq
{
void AnalogEQProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    for (auto& model : wdf)
        model.prepare ((float) spec.sampleRate);

    preSpectrumAnalyserTask.prepareToPlay (spec.sampleRate, (int) spec.maximumBlockSize, (int) spec.numChannels);
    postSpectrumAnalyserTask.prepareToPlay (spec.sampleRate, (int) spec.maximumBlockSize, (int) spec.numChannels);
}

void AnalogEQProcessor::reset()
{
    for (auto& model : wdf)
        model.reset();
}

void AnalogEQProcessor::processBlock (const chowdsp::BufferView<float>& buffer)
{

    if (extraState.isEditorOpen.load() && extraState.showPreSpectrum.get())
        preSpectrumAnalyserTask.processBlockInput (buffer.toAudioBuffer());

    const auto getMagParam = [] (const chowdsp::FloatParameter* param, bool isBoosting)
    {
        const auto normValue = param->convertTo0to1 (param->getCurrentValue());
        return isBoosting ? normValue : 1.0f - normValue;
    };

    const auto numChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();
    for (int ch = 0; ch < numChannels; ++ch)
    {
        wdf[ch].setParameters (getMagParam (params.trebleBoostParam.get(), true),
                               params.trebleBoostQParam->getCurrentValue(),
                               params.trebleBoostFreqParam->getCurrentValue(),
                               getMagParam (params.trebleCutParam.get(), false),
                               params.trebleCutFreqParam->getCurrentValue(),
                               getMagParam (params.bassBoostParam.get(), true),
                               getMagParam (params.bassCutParam.get(), false),
                               params.bassFreqParam->getCurrentValue());

        auto* x = buffer.getWritePointer (ch);
        for (int n = 0; n < numSamples; ++n)
            x[n] = wdf[ch].process (x[n]);
    }

    chowdsp::BufferMath::applyGain (buffer, juce::Decibels::decibelsToGain (22.4f));

    if (extraState.isEditorOpen.load() && extraState.showPostSpectrum.get())
        postSpectrumAnalyserTask.processBlockInput (buffer.toAudioBuffer());
}
} // namespace dsp::analog_eq
