#include "PluginState.h"
#include "NameHelpers.h"
#include "dsp/MultiToolProcessor.h"

#include "dsp/BandSplitter/BandSplitterProcessor.h"
#include "dsp/Brickwall/BrickwallProcessor.h"
#include "dsp/EQ/EQProcessor.h"
#include "dsp/AnalogEQ/AnalogEQProcessor.h"
#include "dsp/SVF/SVFProcessor.h"
#include "dsp/SignalGenerator/SignalGeneratorProcessor.h"
#include "dsp/Waveshaper/WaveshaperProcessor.h"

namespace state
{
PluginParams::PluginParams()
{
    eqParams = std::make_unique<dsp::eq::EQToolParams>();
    waveshaperParams = std::make_unique<dsp::waveshaper::Params>();
    signalGenParams = std::make_unique<dsp::signal_gen::Params>();
    analogEQParams = std::make_unique<dsp::analog_eq::Params>();
    bandSplitParams = std::make_unique<dsp::band_splitter::Params>();
    brickwallParams = std::make_unique<dsp::brickwall::Params>();
    svfParams = std::make_unique<dsp::svf::Params>();

    add (toolParam,
         *eqParams,
         *waveshaperParams,
         *signalGenParams,
         *analogEQParams,
         *bandSplitParams,
         *brickwallParams,
         *svfParams);
}

PluginParams::~PluginParams() = default;

PluginExtraState::PluginExtraState()
{
    waveshaperExtraState = std::make_unique<dsp::waveshaper::ExtraState>();

    addStateValues ({ &waveshaperExtraState->splineState });
}

PluginExtraState::~PluginExtraState() = default;

const juce::StringArray PluginParams::toolChoices = []
{
    juce::StringArray choices { "None" };
    types_list::forEach<dsp::ToolTypes> (
        [&choices] (auto toolTypeIndex)
        {
            using ToolType = typename dsp::ToolTypes::template AtIndex<toolTypeIndex>;
            choices.add (NameHelpers::getNameForProcessor<ToolType>());
        });
    return choices;
}();
} // namespace state
