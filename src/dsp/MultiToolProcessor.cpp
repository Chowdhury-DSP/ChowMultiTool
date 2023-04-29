#include "MultiToolProcessor.h"

namespace dsp
{
namespace detail
{
    template <class U, class F>
    struct tool_maker_t
    {
        operator U() // NOLINT
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
            tool_maker<waveshaper::WaveshaperProcessor> ([&pluginState]
                                                         { return waveshaper::WaveshaperProcessor {
                                                               pluginState,
                                                               *pluginState.params.waveshaperParams,
                                                               *pluginState.nonParams.waveshaperExtraState
                                                           }; }),
            tool_maker<signal_gen::SignalGeneratorProcessor> ([&params]
                                                              { return signal_gen::SignalGeneratorProcessor { *params.signalGenParams }; }),
            tool_maker<analog_eq::AnalogEQProcessor> ([&params]
                                                      { return analog_eq::AnalogEQProcessor { *params.analogEQParams }; }),
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
    for (auto param : std::initializer_list<const juce::RangedAudioParameter*> { params.toolParam.get(),
                                                                                 params.eqParams->linearPhaseMode.get() })
    {
        latencyChangeCallbacks += {
            pluginState.addParameterListener (*param,
                                              chowdsp::ParameterListenerThread::MessageThread,
                                              [this]
                                              { recalculateLatency(); }),
        };
    }
}

void MultiToolProcessor::recalculateLatency()
{
    const auto toolChoice = params.toolParam->getIndex() - 1;
    if (toolChoice < 0) // no tool!
    {
        plugin.setLatencySamples (0);
        return;
    }

    chowdsp::TupleHelpers::visit_at (tools,
                                     (size_t) toolChoice,
                                     [this] (auto& tool)
                                     {
                                         using ToolType [[maybe_unused]] = std::decay_t<decltype (tool)>;
                                         if constexpr (std::is_same_v<ToolType, eq::EQProcessor>)
                                             plugin.setLatencySamples (tool.getLatencySamples());
                                         else
                                             plugin.setLatencySamples (0);
                                     });
}

void MultiToolProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    chowdsp::TupleHelpers::forEachInTuple ([&spec] (auto& tool, size_t)
                                           { tool.prepare (spec); },
                                           tools);
    recalculateLatency();
}

void MultiToolProcessor::processBlock (juce::AudioBuffer<float>& buffer, const juce::MidiBuffer& midiBuffer)
{
    const auto toolChoice = params.toolParam->getIndex() - 1;
    if (toolChoice < 0) // no tool!
    {
        auto busBuffer = plugin.getBusBuffer (buffer, true, 0);
        for (int ch = busBuffer.getNumChannels(); ch < buffer.getNumChannels(); ++ch)
            buffer.clear (ch, 0, buffer.getNumSamples());

        return;
    }

    chowdsp::TupleHelpers::visit_at (tools,
                                     (size_t) toolChoice,
                                     [this, &buffer, &midiBuffer] (auto& tool)
                                     {
                                         using ToolType [[maybe_unused]] = std::decay_t<decltype (tool)>;
                                         if constexpr (std::is_same_v<ToolType, band_splitter::BandSplitterProcessor>)
                                         {
                                             const auto outputBusCount = plugin.getBusCount (false);
                                             if (outputBusCount != 4 || buffer.getNumChannels() < 4)
                                                 return;

                                             const auto inBuffer = plugin.getBusBuffer (buffer, true, 0);
                                             auto lowBuffer = plugin.getBusBuffer (buffer, false, 1);
                                             auto midBuffer = plugin.getBusBuffer (buffer, false, 2);
                                             auto highBuffer = plugin.getBusBuffer (buffer, false, 3);
                                             tool.processBlock (inBuffer, lowBuffer, midBuffer, highBuffer);
                                         }
                                         else if constexpr (std::is_same_v<ToolType, svf::SVFProcessor>)
                                         {
                                             auto busBuffer = plugin.getBusBuffer (buffer, true, 0);
                                             tool.processBlock (busBuffer, midiBuffer);

                                             for (int ch = busBuffer.getNumChannels(); ch < buffer.getNumChannels(); ++ch)
                                                 buffer.clear (ch, 0, buffer.getNumSamples());
                                         }
                                         else
                                         {
                                             auto busBuffer = plugin.getBusBuffer (buffer, true, 0);
                                             tool.processBlock (busBuffer);

                                             for (int ch = busBuffer.getNumChannels(); ch < buffer.getNumChannels(); ++ch)
                                                 buffer.clear (ch, 0, buffer.getNumSamples());
                                         }
                                     });
}
} // namespace dsp
