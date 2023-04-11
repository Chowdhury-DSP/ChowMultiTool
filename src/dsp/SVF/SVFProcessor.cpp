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

    cutoffSmooth.reset (spec.sampleRate, 0.025);
    cutoffSmooth.setCurrentAndTargetValue (*params.cutoff);
    qSmooth.reset (spec.sampleRate, 0.025);
    qSmooth.setCurrentAndTargetValue (*params.qParam);
    modeSmooth.reset (spec.sampleRate, 0.025);
    modeSmooth.setCurrentAndTargetValue (*params.mode);
    dampingSmooth.reset (spec.sampleRate, 0.025);
    dampingSmooth.setCurrentAndTargetValue (*params.wernerDamping);
}

void SVFProcessor::reset()
{
    chowdsp::TupleHelpers::forEachInTuple ([] (auto& filter, size_t)
                                           { filter.reset(); },
                                           plainFilters);
    arpFilter.reset();
    wernerFilter.reset();

    cutoffSmooth.setCurrentAndTargetValue (*params.cutoff);
    qSmooth.setCurrentAndTargetValue (*params.qParam);
    modeSmooth.setCurrentAndTargetValue (*params.mode);
    dampingSmooth.setCurrentAndTargetValue (*params.wernerDamping);
}

template <typename FilterType, typename... TestFilterTypes>
constexpr bool IsOneOfFilters = std::disjunction<std::is_same<FilterType, TestFilterTypes>...>::value;

void SVFProcessor::processBlock (const chowdsp::BufferView<float>& buffer) noexcept
{
    cutoffSmooth.setTargetValue (*params.cutoff);
    qSmooth.setTargetValue (*params.qParam);
    modeSmooth.setTargetValue (*params.mode);
    dampingSmooth.setTargetValue (*params.wernerDamping);

    static constexpr int smallBlockSize = 32;

    auto sampleCount = 0;
    auto numSamplesRemaining = buffer.getNumSamples();
    while (numSamplesRemaining > 0)
    {
        const auto samplesToProcess = juce::jmin (smallBlockSize, numSamplesRemaining);
        processSmallBlock (chowdsp::BufferView<float> { buffer, sampleCount, samplesToProcess });

        sampleCount += samplesToProcess;
        numSamplesRemaining -= samplesToProcess;
    }
}

void SVFProcessor::processSmallBlock (const chowdsp::BufferView<float>& buffer) noexcept
{
    const auto numSamples = buffer.getNumSamples();
    cutoffSmooth.skip (numSamples);
    qSmooth.skip (numSamples);
    modeSmooth.skip (numSamples);
    dampingSmooth.skip (numSamples);

    const auto setFilterParams = [this] (auto& filter)
    {
        using FilterType = std::decay_t<decltype (filter)>;

        filter.template setCutoffFrequency<false> (cutoffSmooth.getCurrentValue());
        filter.template setQValue<false> (qSmooth.getCurrentValue());

        if constexpr (IsOneOfFilters<FilterType, chowdsp::SVFMultiMode<>>)
            filter.setMode (0.5f + 0.5f * modeSmooth.getCurrentValue());

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
        processFilterType (plainFilters, params.plainType->getIndex());
    }
    else if (params.type->get() == SVFType::ARP)
    {
        setFilterParams (arpFilter);
        magic_enum::enum_switch (
            [this, &buffer] (auto subType)
            {
                constexpr chowdsp::ARPFilterType type = subType;
                arpFilter.template processBlock<type> (buffer, -modeSmooth.getCurrentValue());
            },
            params.arpType->get());
    }
    else if (params.type->get() == SVFType::Werner)
    {
        const auto resonance = params.qParam->convertTo0to1 (qSmooth.getCurrentValue());
        wernerFilter.calcCoeffs (cutoffSmooth.getCurrentValue(), dampingSmooth.getCurrentValue(), resonance);

        magic_enum::enum_switch (
            [this, &buffer] (auto subType)
            {
                constexpr chowdsp::WernerFilterType type = subType;
                if constexpr (type == chowdsp::WernerFilterType::MultiMode)
                {
                    const auto mix = 0.5f + 0.5f * modeSmooth.getCurrentValue();
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
