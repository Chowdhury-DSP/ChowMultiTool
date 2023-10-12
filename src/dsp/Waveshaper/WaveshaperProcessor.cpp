#include "WaveshaperProcessor.h"

namespace dsp::waveshaper
{
const auto MRange = chowdsp::ParamUtils::createNormalisableRange (5.0f, 50.0f, 15.0f);

WaveshaperProcessor::WaveshaperProcessor (chowdsp::PluginState& state, Params& wsParams, ExtraState& wsExtraState)
    : params (wsParams),
      freeDrawShaper (wsExtraState.freeDrawState),
      mathShaper (wsExtraState.mathState),
      pointsShaper (wsExtraState.pointsState)
{
    osChangeCallback = state.addParameterListener (*params.oversampleParam,
                                                   chowdsp::ParameterListenerThread::MessageThread,
                                                   [this]
                                                   {
                                                       oversamplingRateChanged();
                                                   });
}

void WaveshaperProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;

    gain.setRampDurationSeconds (0.05);
    gain.prepare (spec);

    adaaHardClipper.prepare ((int) spec.numChannels);
    adaaTanhClipper.prepare ((int) spec.numChannels);
    adaaCubicClipper.prepare ((int) spec.numChannels);
    adaa9thOrderClipper.prepare ((int) spec.numChannels);
    fullWaveRectifier.prepare ((int) spec.numChannels);
    westCoastFolder.prepare ((int) spec.numChannels);
    waveMultiplyFolder.prepare ((int) spec.numChannels);
    freeDrawShaper.prepare (spec);
    mathShaper.prepare (spec);
    pointsShaper.prepare (spec);
    clipGuard.prepare (spec);

    static constexpr auto maxOSRatio = static_cast<int> (magic_enum::enum_values<OversamplingRatio>().back());
    doubleBuffer.setMaxSize ((int) spec.numChannels, (int) spec.maximumBlockSize * maxOSRatio);
    doubleSIMDBuffer.setMaxSize (chowdsp::Math::ceiling_divide ((int) spec.numChannels, (int) xsimd::batch<double>::size), (int) spec.maximumBlockSize * maxOSRatio);

    oversamplingRateChanged();
}

void WaveshaperProcessor::oversamplingRateChanged()
{
    const juce::SpinLock::ScopedLockType lock { processingMutex };

    const auto osRatio = static_cast<int> (params.oversampleParam->get());
    upsampler.prepare (processSpec, osRatio);

    auto osProcessSpec = processSpec;
    osProcessSpec.sampleRate *= (double) osRatio;
    osProcessSpec.maximumBlockSize *= (uint32_t) osRatio;
    downsampler.prepare (osProcessSpec, osRatio);

    ssWaveshaper.prepare (processSpec.sampleRate * osRatio,
                          (int) processSpec.maximumBlockSize * osRatio,
                          (int) processSpec.numChannels);
}

void WaveshaperProcessor::processBlock (const chowdsp::BufferView<float>& buffer)
{
    const juce::SpinLock::ScopedTryLockType tryLock { processingMutex };
    if (! tryLock.isLocked())
        return;

    gain.setGainDecibels (params.gainParam->getCurrentValue());
    gain.process (buffer);

    const auto osBufferView = upsampler.process (buffer);

    const auto clipLevel = juce::Decibels::decibelsToGain (18.0f);
    for (auto [ch, data] : chowdsp::buffer_iters::channels (osBufferView))
        juce::FloatVectorOperations::clip (data.data(), data.data(), -clipLevel, clipLevel, data.size());

    doubleBuffer.setCurrentSize (osBufferView.getNumChannels(), osBufferView.getNumSamples());
    chowdsp::BufferMath::copyBufferData (osBufferView, doubleBuffer);

    const auto shapeParam = params.shapeParam->get();
    if (shapeParam == Shapes::Hard_Clip)
        adaaHardClipper.processBlock (doubleBuffer);
    else if (shapeParam == Shapes::Tanh_Clip)
        adaaTanhClipper.processBlock (doubleBuffer);
    else if (shapeParam == Shapes::Cubic_Clip)
        adaaCubicClipper.processBlock (doubleBuffer);
    else if (shapeParam == Shapes::Nonic_Clip)
        adaa9thOrderClipper.processBlock (doubleBuffer);
    else if (shapeParam == Shapes::Full_Wave_Rectify)
        fullWaveRectifier.processBlock (doubleBuffer);
    else if (shapeParam == Shapes::West_Coast)
    {
        westCoastFolder.processBlock (doubleBuffer);
        chowdsp::BufferMath::applyGain (doubleBuffer, juce::Decibels::decibelsToGain (-10.0));
    }
    else if (shapeParam == Shapes::Wave_Multiply)
    {
        waveMultiplyFolder.processBlock (doubleBuffer);
        chowdsp::BufferMath::applyGain (doubleBuffer, juce::Decibels::decibelsToGain (16.0));
    }
    else if (shapeParam == Shapes::Fold_Fuzz)
    {
        chowdsp::copyToSIMDBuffer (doubleBuffer, doubleSIMDBuffer);
        ssWaveshaper.processBlock (doubleSIMDBuffer,
                                   params.kParam->getCurrentValue(),
                                   MRange.convertFrom0to1 (params.MParam->getCurrentValue()));
        chowdsp::copyFromSIMDBuffer (doubleSIMDBuffer, doubleBuffer);
    }
    else if (shapeParam == Shapes::Free_Draw)
    {
        freeDrawShaper.processBlock (doubleBuffer);
    }
    else if (shapeParam == Shapes::Math)
    {
        mathShaper.processBlock (doubleBuffer);
    }
    else if (shapeParam == Shapes::Spline)
    {
        pointsShaper.processBlock (doubleBuffer);
    }

    chowdsp::BufferMath::copyBufferData (doubleBuffer, osBufferView);

    downsampler.process (osBufferView, buffer);

    clipGuard.setCeiling ((shapeParam == Shapes::Hard_Clip
                           || shapeParam == Shapes::Tanh_Clip
                           || shapeParam == Shapes::Cubic_Clip
                           || shapeParam == Shapes::Nonic_Clip)
                              ? 1.0f
                              : 100.0f);
    clipGuard.processBlock (buffer);
}
} // namespace dsp::waveshaper
