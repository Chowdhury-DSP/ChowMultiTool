#include "BrickwallProcessor.h"

namespace dsp::brickwall
{
void BrickwallProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    filter.prepare (spec);
    filter.setQValue (chowdsp::CoefficientCalculators::butterworthQ<float>);
    postSpectrumAnalyserTask.prepareToPlay (spec.sampleRate, (int) spec.maximumBlockSize, (int) spec.numChannels);

    const auto paddedChannelSize = chowdsp::Math::round_to_next_multiple (static_cast<size_t> (spec.maximumBlockSize),
                                                                          chowdsp::SIMDUtils::defaultSIMDAlignment);
    const auto requiredMemoryBytes = paddedChannelSize * sizeof (float) * 3 // per-band smoothed values
                                     + paddedChannelSize * spec.numChannels * sizeof (float); // per-band fade buffers
    arena.reset (requiredMemoryBytes + 32);
}

void BrickwallProcessor::reset()
{
    filter.setFilterType (getFilterTypeIndex());
    filter.setCutoffFrequency (*params.cutoff);
    filter.reset();
}

void BrickwallProcessor::processBlock (const chowdsp::BufferView<float>& buffer) noexcept
{
    filter.setFilterType (getFilterTypeIndex());
    filter.setCutoffFrequency (*params.cutoff);
    filter.processBlock (buffer, arena);

    if (extraState.isEditorOpen.load() && extraState.showSpectrum.get())
        postSpectrumAnalyserTask.processBlockInput (buffer.toAudioBuffer());
}

int BrickwallProcessor::getFilterTypeIndex() const
{
    return static_cast<int> ((size_t) params.filterType->getIndex() * magic_enum::enum_count<FilterMode>() * magic_enum::enum_count<Order>()
                             + (size_t) params.order->getIndex() * magic_enum::enum_count<FilterMode>()
                             + (size_t) params.filterMode->getIndex());
}
} // namespace dsp::brickwall
