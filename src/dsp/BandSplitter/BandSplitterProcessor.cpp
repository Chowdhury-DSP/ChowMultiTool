#include "BandSplitterProcessor.h"

namespace dsp::band_splitter
{
void BandSplitterProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    filter1.prepare (spec);
    filter2.prepare (spec);
    filter4.prepare (spec);
    filter8.prepare (spec);
    filter12.prepare (spec);
}

void BandSplitterProcessor::processBlock (const chowdsp::BufferView<const float>& bufferIn,
                                          const chowdsp::BufferView<float>& bufferLow,
                                          const chowdsp::BufferView<float>& bufferHigh)
{
    const auto processFilter = [&] (auto& filter)
    {
        filter.setCrossoverFrequency (params->cutoff->getCurrentValue());
        filter.processBlock (bufferIn, bufferLow, bufferHigh);
    };

    const auto slope = params->slope->get();
    if (slope == Slope::m6_dBpOct)
        processFilter (filter1);
    else if (slope == Slope::m12_dBpOct)
        processFilter (filter2);
    else if (slope == Slope::m24_dBpOct)
        processFilter (filter4);
    else if (slope == Slope::m48_dBpOct)
        processFilter (filter8);
    else if (slope == Slope::m72_dBpOct)
        processFilter (filter12);
}
} // namespace dsp::band_splitter
