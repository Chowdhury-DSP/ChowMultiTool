#include "WaveshaperProcessor.h"

namespace dsp::waveshaper
{
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
}

void WaveshaperProcessor::processBlock (const chowdsp::BufferView<float>& buffer)
{
    gain.setGainDecibels (params.gainParam->getCurrentValue());
    gain.process (buffer);

    // @TODO: oversampling

    if (params.shapeParam->get() == Shapes::Hard_Clip)
        adaaHardClipper.processBlock (buffer);
    else if (params.shapeParam->get() == Shapes::Tanh_Clip)
        adaaTanhClipper.processBlock (buffer);
    else if (params.shapeParam->get() == Shapes::Cubic_Clip)
        adaaCubicClipper.processBlock (buffer);
    else if (params.shapeParam->get() == Shapes::Nonic_Clip)
        adaa9thOrderClipper.processBlock (buffer);
    else if (params.shapeParam->get() == Shapes::Full_Wave_Rectify)
        fullWaveRectifier.processBlock (buffer);
    else if (params.shapeParam->get() == Shapes::West_Coast)
        westCoastFolder.processBlock (buffer);
    else if (params.shapeParam->get() == Shapes::Wave_Multiply)
        waveMultiplyFolder.processBlock (buffer);
}
} // namespace dsp::waveshaper
