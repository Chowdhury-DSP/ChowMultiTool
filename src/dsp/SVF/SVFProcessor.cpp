#include "SVFProcessor.h"
#include <chowdsp_reflection/third_party/magic_enum/include/magic_enum_switch.hpp>

namespace dsp::svf
{
void SVFProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    chowdsp::TupleHelpers::forEachInTuple ([&spec] (auto& filter, size_t)
                                           { filter.prepare (spec); },
                                           plainFilters);
    arpFilter.prepare (spec);
    wernerFilter.prepare (spec);
}

void SVFProcessor::reset()
{
    chowdsp::TupleHelpers::forEachInTuple ([] (auto& filter, size_t)
                                           { filter.reset(); },
                                           plainFilters);
    arpFilter.reset();
    wernerFilter.reset();
}

template <typename FilterType, typename... TestFilterTypes>
constexpr bool IsOneOfFilters = std::disjunction<std::is_same<FilterType, TestFilterTypes>...>::value;

void SVFProcessor::processBlock (const chowdsp::BufferView<float>& buffer)
{
    const auto setFilterParams = [this] (auto& filter)
    {
        using FilterType = std::decay_t<decltype (filter)>;

        filter.template setCutoffFrequency<false> (*params.cutoff);
        filter.template setQValue<false> (*params.qParam);

        if constexpr (IsOneOfFilters<FilterType, chowdsp::SVFBell<>, chowdsp::SVFLowShelf<>, chowdsp::SVFHighShelf<>>)
            filter.template setGainDecibels<false> (*params.gain);

        if constexpr (IsOneOfFilters<FilterType, chowdsp::SVFMultiMode<>>)
            filter.setMode (0.5f + 0.5f * *params.mode); // TODO: maybe smooth this parameter

        if constexpr (IsOneOfFilters<FilterType, chowdsp::ARPFilter<float>>)
            filter.setLimitMode (*params.arpLimitMode);

        filter.update();
    };

    const auto processFilterType = [&setFilterParams, &buffer] (auto& filtersTuple, auto filterSubType)
    {
        chowdsp::TupleHelpers::visit_at (
            filtersTuple,
            static_cast<size_t> (filterSubType),
            [&setFilterParams, &buffer] (auto& filter)
            {
                setFilterParams (filter);
                filter.processBlock (buffer);
            });
    };

    if (params.type->get() == SVFType::Plain)
    {
        processFilterType (plainFilters, params.plainType->get());
    }
    else if (params.type->get() == SVFType::ARP)
    {
        setFilterParams (arpFilter);
        magic_enum::enum_switch (
            [this, &buffer] (auto subType)
            {
                constexpr chowdsp::ARPFilterType type = subType;
                arpFilter.template processBlock<type> (buffer, -(*params.mode));
            },
            params.arpType->get());
    }
    else if (params.type->get() == SVFType::Werner)
    {
        const auto resonance = params.qParam->convertTo0to1 (*params.qParam);
        wernerFilter.calcCoeffs (*params.cutoff, *params.wernerDamping, resonance);

        magic_enum::enum_switch (
            [this, &buffer] (auto subType)
            {
                constexpr chowdsp::WernerFilterType type = subType;
                if constexpr (type == chowdsp::WernerFilterType::MultiMode)
                {
                    const auto mix = 0.5f + 0.5f * *params.mode;
                    wernerFilter.template processBlock<type> (buffer, mix);
                }
                else
                {
                    wernerFilter.template processBlock<type> (buffer);
                }
            },
            params.wernerType->get());
    }
}
} // namespace dsp::svf
