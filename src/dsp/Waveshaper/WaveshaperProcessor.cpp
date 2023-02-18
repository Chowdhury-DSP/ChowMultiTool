#include "WaveshaperProcessor.h"

namespace dsp::waveshaper
{
const auto MRange = chowdsp::ParamUtils::createNormalisableRange (5.0f, 50.0f, 15.0f);

void WaveshaperProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    gain.setRampDurationSeconds (0.05);
    gain.prepare (spec);

    adaaHardClipper.prepare ((int) spec.numChannels);
    adaaTanhClipper.prepare ((int) spec.numChannels);
    adaaCubicClipper.prepare ((int) spec.numChannels);
    adaa9thOrderClipper.prepare ((int) spec.numChannels);
    fullWaveRectifier.prepare ((int) spec.numChannels);
    westCoastFolder.prepare ((int) spec.numChannels);
    waveMultiplyFolder.prepare ((int) spec.numChannels);
    ssWaveshaper.prepare (spec.sampleRate, (int) spec.maximumBlockSize, (int) spec.numChannels);

    doubleBuffer.setMaxSize ((int) spec.numChannels, (int) spec.maximumBlockSize);
    doubleSIMDBuffer.setMaxSize (chowdsp::Math::ceiling_divide ((int) spec.numChannels, (int) xsimd::batch<double>::size), (int) spec.maximumBlockSize);
}

void WaveshaperProcessor::processBlock (const chowdsp::BufferView<float>& buffer)
{
    gain.setGainDecibels (params.gainParam->getCurrentValue());
    gain.process (buffer);

    // @TODO: oversampling

    doubleBuffer.setCurrentSize (buffer.getNumChannels(), buffer.getNumSamples());
    chowdsp::BufferMath::copyBufferData (buffer, doubleBuffer);

    if (params.shapeParam->get() == Shapes::Hard_Clip)
        adaaHardClipper.processBlock (doubleBuffer);
    else if (params.shapeParam->get() == Shapes::Tanh_Clip)
        adaaTanhClipper.processBlock (doubleBuffer);
    else if (params.shapeParam->get() == Shapes::Cubic_Clip)
        adaaCubicClipper.processBlock (doubleBuffer);
    else if (params.shapeParam->get() == Shapes::Nonic_Clip)
        adaa9thOrderClipper.processBlock (doubleBuffer);
    else if (params.shapeParam->get() == Shapes::Full_Wave_Rectify)
        fullWaveRectifier.processBlock (doubleBuffer);
    else if (params.shapeParam->get() == Shapes::West_Coast)
    {
        westCoastFolder.processBlock (doubleBuffer);
        chowdsp::BufferMath::applyGain (doubleBuffer, juce::Decibels::decibelsToGain (-10.0));
    }
    else if (params.shapeParam->get() == Shapes::Wave_Multiply)
    {
        waveMultiplyFolder.processBlock (doubleBuffer);
        chowdsp::BufferMath::applyGain (doubleBuffer, juce::Decibels::decibelsToGain (16.0));
    }
    else if (params.shapeParam->get() == Shapes::Fold_Fuzz)
    {
        chowdsp::copyToSIMDBuffer (doubleBuffer, doubleSIMDBuffer);
        ssWaveshaper.processBlock (doubleSIMDBuffer,
                                   params.kParam->getCurrentValue(),
                                   MRange.convertFrom0to1 (params.MParam->getCurrentValue()));
        chowdsp::copyFromSIMDBuffer (doubleSIMDBuffer, doubleBuffer);
    }

    chowdsp::BufferMath::copyBufferData (doubleBuffer, buffer);
}
} // namespace dsp::waveshaper
