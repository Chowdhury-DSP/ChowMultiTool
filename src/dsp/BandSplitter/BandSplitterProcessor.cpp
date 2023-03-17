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
        processCrossover (threeBandFilters, processThreeBandFilter);
    else
        processCrossover (twoBandFilters, processTwoBandFilter);
}
} // namespace dsp::band_splitter
