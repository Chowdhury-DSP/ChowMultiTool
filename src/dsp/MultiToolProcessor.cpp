#include "MultiToolProcessor.h"

namespace dsp
{
namespace detail
{
    template <class U, class F>
    struct tool_maker_t
    {
        operator U()
        {
            return f();
        }
        F f;
    };

    template <class U, class F>
    tool_maker_t<U, std::decay_t<F>> tool_maker (F&& f)
    {
        return { std::forward<F> (f) };
    }

    ToolTypes::Types generate_tools (State& pluginState)
    {
        const auto& params = pluginState.params;
        return {
            tool_maker<eq::EQProcessor> ([&params]
                                         { return eq::EQProcessor { *params.eqParams }; }),
            tool_maker<waveshaper::WaveshaperProcessor> ([&pluginState, &params]
                                                         { return waveshaper::WaveshaperProcessor { pluginState, *params.waveshaperParams }; }),
            tool_maker<signal_gen::SignalGeneratorProcessor> ([&params]
                                                              { return signal_gen::SignalGeneratorProcessor { *params.signalGenParams }; }),
            tool_maker<pultec::PultecEQProcessor> ([&params]
                                                   { return pultec::PultecEQProcessor { *params.pultecEQParams }; }),
            tool_maker<band_splitter::BandSplitterProcessor> ([&params]
                                                              { return band_splitter::BandSplitterProcessor { *params.bandSplitParams }; }),
            tool_maker<brickwall::BrickwallProcessor> ([&params]
                                                       { return brickwall::BrickwallProcessor { *params.brickwallParams }; }),
            tool_maker<svf::SVFProcessor> ([&params]
                                           { return svf::SVFProcessor { *params.svfParams }; })
        };
    }
} // namespace detail

MultiToolProcessor::MultiToolProcessor (juce::AudioProcessor& parentPlugin, State& pluginState)
    : plugin (parentPlugin),
      params (pluginState.params),
      tools (detail::generate_tools (pluginState))
{
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
