#include "BrickwallProcessor.h"

namespace dsp::brickwall
{
void BrickwallProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    filter.prepare (spec);
    filter.setQValue (chowdsp::CoefficientCalculators::butterworthQ<float>);
}

void BrickwallProcessor::processBlock (const chowdsp::BufferView<float>& buffer) noexcept
{
    filter.setFilterType (getFilterTypeIndex());
    filter.setCutoffFrequency (*params->cutoff);
    filter.processBlock (buffer);
}

int BrickwallProcessor::getFilterTypeIndex() const
{
    return static_cast<int> ((size_t) params->filterType->get() * magic_enum::enum_count<FilterMode>() * magic_enum::enum_count<Order>()
                             + (size_t) params->order->get() * magic_enum::enum_count<FilterMode>()
                             + (size_t) params->filterMode->get());
}
} // namespace dsp::brickwall
