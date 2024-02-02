#include "PluginState.h"
#include "NameHelpers.h"
#include "dsp/MultiToolProcessor.h"

#include "dsp/AnalogEQ/AnalogEQProcessor.h"
#include "dsp/BandSplitter/BandSplitterProcessor.h"
#include "dsp/Brickwall/BrickwallProcessor.h"
#include "dsp/EQ/EQProcessor.h"
#include "dsp/SVF/SVFProcessor.h"
#include "dsp/SignalGenerator/SignalGeneratorProcessor.h"
#include "dsp/Waveshaper/WaveshaperProcessor.h"

namespace state
{
PluginParams::PluginParams()
{
    // useful for getting type sizes:
    //    std::cout << sizeof (dsp::eq::EQToolParams) << std::endl;
    //    std::cout << sizeof (dsp::waveshaper::Params) << std::endl;
    //    std::cout << sizeof (dsp::signal_gen::Params) << std::endl;
    //    std::cout << sizeof (dsp::analog_eq::Params) << std::endl;
    //    std::cout << sizeof (dsp::band_splitter::Params) << std::endl;
    //    std::cout << sizeof (dsp::brickwall::Params) << std::endl;
    //    std::cout << sizeof (dsp::svf::Params) << std::endl;

    eqParams = std::make_unique<dsp::eq::EQToolParams>();
    waveshaperParams.emplace();
    signalGenParams.emplace();
    analogEQParams.emplace();
    bandSplitParams.emplace();
    brickwallParams.emplace();
    svfParams.emplace();

    add (toolParam,
         bypassParam,
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
    analogEqExtraState.emplace();
    eqExtraState.emplace();
    brickwallExtraState.emplace();
    svfExtraState.emplace();
    bandSplitterExtraState.emplace();

    addStateValues ({ &waveshaperExtraState->freeDrawState,
                      &waveshaperExtraState->mathState,
                      &waveshaperExtraState->pointsState,
                      &analogEqExtraState->showPreSpectrum,
                      &analogEqExtraState->showPostSpectrum,
                      &eqExtraState->showPreSpectrum,
                      &eqExtraState->showPostSpectrum,
                      &brickwallExtraState->showSpectrum,
                      &svfExtraState->showSpectrum,
                      &bandSplitterExtraState->showSpectrum,
                      &editorBounds });
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
