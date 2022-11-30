#include "MultiToolProcessor.h"

namespace dsp
{
MultiToolProcessor::MultiToolProcessor (state::PluginParams& pluginParams)
    : params (pluginParams)
{
    chowdsp::TupleHelpers::forEachInTuple ([this] (auto& tool, size_t)
                                           {
                                               tool.initialise (params);
                                           }, tools);
}

void MultiToolProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    chowdsp::TupleHelpers::forEachInTuple ([&spec] (auto& tool, size_t)
                                           {
                                              tool.prepare (spec);
                                           }, tools);
}

void MultiToolProcessor::processBlock (const chowdsp::BufferView<float>& buffer)
{
    const auto toolChoice = params.toolParam->getIndex() - 1;
    if (toolChoice < 0)
        return; // no tool!

    chowdsp::TupleHelpers::visit_at (tools, (size_t) toolChoice, [&buffer] (auto& tool)
                                     {
                                         tool.processBlock (buffer);
                                     });
}
} // namespace dsp
