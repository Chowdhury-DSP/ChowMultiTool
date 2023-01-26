#include "MultiToolProcessor.h"

namespace dsp
{
MultiToolProcessor::MultiToolProcessor (juce::AudioProcessor& parentPlugin, state::PluginParams& pluginParams)
    : plugin (parentPlugin),
      params (pluginParams)
{
    chowdsp::TupleHelpers::forEachInTuple ([this] (auto& tool, size_t)
                                           { tool.initialise (params); },
                                           tools);
}

void MultiToolProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    chowdsp::TupleHelpers::forEachInTuple ([&spec] (auto& tool, size_t)
                                           { tool.prepare (spec); },
                                           tools);
}

void MultiToolProcessor::processBlock (juce::AudioBuffer<float>& buffer)
{
    const auto toolChoice = params.toolParam->getIndex() - 1;
    if (toolChoice < 0)
        return; // no tool!

    chowdsp::TupleHelpers::visit_at (tools,
                                     (size_t) toolChoice,
                                     [this, &buffer] (auto& tool)
                                     {
                                         using ToolType [[maybe_unused]] = std::decay_t<decltype (tool)>;
                                         if constexpr (std::is_same_v<ToolType, band_splitter::BandSplitterProcessor>)
                                         {
                                             if (plugin.getBusCount (false) != 3)
                                                 return;

                                             const auto inBuffer = plugin.getBusBuffer (buffer, true, 0);
                                             auto lowBuffer = plugin.getBusBuffer (buffer, false, 1);
                                             auto highBuffer = plugin.getBusBuffer (buffer, false, 2);
                                             tool.processBlock (inBuffer, lowBuffer, highBuffer);
                                         }
                                         else
                                         {
                                             auto busBuffer = plugin.getBusBuffer (buffer, true, 0);
                                             tool.processBlock (busBuffer);
                                         }
                                     });
}
} // namespace dsp
