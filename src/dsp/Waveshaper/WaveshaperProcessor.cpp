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

    static constexpr auto maxOSRatio = static_cast<int> (magic_enum::enum_values<OversamplingRatio>().back());
    doubleBuffer.setMaxSize ((int) spec.numChannels, (int) spec.maximumBlockSize * maxOSRatio);
    doubleSIMDBuffer.setMaxSize (chowdsp::Math::ceiling_divide ((int) spec.numChannels, (int) xsimd::batch<double>::size), (int) spec.maximumBlockSize * maxOSRatio);

    oversamplingRateChanged();
}

void WaveshaperProcessor::oversamplingRateChanged()
{
    const juce::SpinLock::ScopedLockType lock { processingMutex };

    const auto osRatio = static_cast<int> (params.oversampleParam->get());
    if (osRatio > 1)
    {
        upsampler.prepare (processSpec, osRatio);

        auto osProcessSpec = processSpec;
        osProcessSpec.sampleRate *= (double) osRatio;
        osProcessSpec.maximumBlockSize *= (uint32_t) osRatio;
        downsampler.prepare (osProcessSpec, osRatio);
    }

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

    const auto osRatio = static_cast<int> (params.oversampleParam->get());
    std::optional<chowdsp::BufferView<float>> osBufferView;
    if (osRatio > 1)
    {
        osBufferView.emplace (upsampler.process (buffer), 0, -1);
    }
    else
    {
        osBufferView.emplace (buffer, 0, -1);
    }
    doubleBuffer.setCurrentSize (osBufferView->getNumChannels(), osBufferView->getNumSamples());
    chowdsp::BufferMath::copyBufferData (*osBufferView, doubleBuffer);

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
    else if (params.shapeParam->get() == Shapes::Free_Draw)
    {
        freeDrawShaper.processBlock (doubleBuffer);
    }
    else if (params.shapeParam->get() == Shapes::Math)
    {
        mathShaper.processBlock (doubleBuffer);
    }
    else if (params.shapeParam->get() == Shapes::Spline)
    {
        pointsShaper.processBlock (doubleBuffer);
    }

    chowdsp::BufferMath::copyBufferData (doubleBuffer, *osBufferView);

    if (osRatio > 1)
    {
        const auto dsBuffer = downsampler.process (*osBufferView);
        chowdsp::BufferMath::copyBufferData (dsBuffer, buffer);
    }
}
} // namespace dsp::waveshaper
