#include "BandSplitterProcessor.h"

namespace dsp::band_splitter
{
void BandSplitterProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    pfr::for_each_field (twoBandFilters,
                         [spec] (auto&& filter)
                         { filter.prepare (spec); });

    pfr::for_each_field (threeBandFilters,
                         [spec] (auto&& filter)
                         { filter.prepare (spec); });

    lowPreSpectrumAnalyserTask.prepareToPlay (spec.sampleRate, (int) spec.maximumBlockSize, (int) spec.numChannels);
    lowPostSpectrumAnalyserTask.prepareToPlay (spec.sampleRate, (int) spec.maximumBlockSize, (int) spec.numChannels);
    midPreSpectrumAnalyserTask.prepareToPlay (spec.sampleRate, (int) spec.maximumBlockSize, (int) spec.numChannels);
    midPostSpectrumAnalyserTask.prepareToPlay (spec.sampleRate, (int) spec.maximumBlockSize, (int) spec.numChannels);
    highPreSpectrumAnalyserTask.prepareToPlay (spec.sampleRate, (int) spec.maximumBlockSize, (int) spec.numChannels);
    highPostSpectrumAnalyserTask.prepareToPlay (spec.sampleRate, (int) spec.maximumBlockSize, (int) spec.numChannels);
}

void BandSplitterProcessor::processBlock (const chowdsp::BufferView<const float>& bufferIn,
                                          const chowdsp::BufferView<float>& bufferLow,
                                          const chowdsp::BufferView<float>& bufferMid,
                                          const chowdsp::BufferView<float>& bufferHigh)
{
    if (bufferIn.getNumChannels() != bufferLow.getNumChannels()
        || bufferIn.getNumChannels() != bufferMid.getNumChannels()
        || bufferIn.getNumChannels() != bufferHigh.getNumChannels())
        return;

    if (bufferLow.getReadPointer (0) == nullptr
        || bufferMid.getReadPointer (0) == nullptr
        || bufferHigh.getReadPointer (0) == nullptr)
        return;

    if (extraState.isEditorOpen.load() && extraState.showPreSpectrum.get())
    {
        lowPreSpectrumAnalyserTask.processBlockInput (bufferLow.toAudioBuffer());
        midPreSpectrumAnalyserTask.processBlockInput (bufferMid.toAudioBuffer());
        highPreSpectrumAnalyserTask.processBlockInput (bufferHigh.toAudioBuffer());
    }

    const auto processTwoBandFilter = [&] (auto& filter)
    {
        filter.setCrossoverFrequency (params.cutoff->getCurrentValue());
        filter.processBlock (bufferIn, bufferLow, bufferHigh);
    };

    const auto processThreeBandFilter = [&] (auto& filter)
    {
        filter.setLowCrossoverFrequency (params.cutoff->getCurrentValue());
        filter.setHighCrossoverFrequency (params.cutoff2->getCurrentValue());
        filter.processBlock (bufferIn, bufferLow, bufferMid, bufferHigh);
    };

    const auto processCrossover = [&] (auto& bandFilters, auto& processFilter)
    {
        const auto slope = params.slope->get();
        if (slope == Slope::m6_dBpOct)
            processFilter (bandFilters.filter1);
        else if (slope == Slope::m12_dBpOct)
            processFilter (bandFilters.filter2);
        else if (slope == Slope::m24_dBpOct)
            processFilter (bandFilters.filter4);
        else if (slope == Slope::m48_dBpOct)
            processFilter (bandFilters.filter8);
        else if (slope == Slope::m72_dBpOct)
            processFilter (bandFilters.filter12);
    };

    if (params.threeBandOnOff->get())
    {
        processCrossover (threeBandFilters, processThreeBandFilter);
        if (extraState.isEditorOpen.load() && extraState.showPostSpectrum.get())
        {
            lowPostSpectrumAnalyserTask.processBlockInput(bufferLow.toAudioBuffer());
            midPostSpectrumAnalyserTask.processBlockInput (bufferMid.toAudioBuffer());
            highPostSpectrumAnalyserTask.processBlockInput (bufferHigh.toAudioBuffer());
        }
    }
    else
    {
        bufferMid.clear();
        processCrossover (twoBandFilters, processTwoBandFilter);
        if (extraState.isEditorOpen.load() && extraState.showPostSpectrum.get())
        {
            lowPostSpectrumAnalyserTask.processBlockInput (bufferLow.toAudioBuffer());
            highPostSpectrumAnalyserTask.processBlockInput (bufferHigh.toAudioBuffer());
        }
    }
}
} // namespace dsp::band_splitter
