#include "PluginRemoteControls.h"
#include "dsp/MultiToolProcessor.h"

namespace state
{
using ToolTypes = dsp::ToolTypes;

RemoteControlsHelper::RemoteControlsHelper (chowdsp::PluginState& state, PluginParams& pluginParams, clap_juce_extensions::clap_juce_audio_processor_capabilities* cje)
    : params (pluginParams)
{
    toolChangeCallback = state.addParameterListener (pluginParams.toolParam,
                                                     chowdsp::ParameterListenerThread::MessageThread,
                                                     [cje, &toolParam = pluginParams.toolParam]
                                                     {
                                                         cje->suggestRemoteControlsPage ((uint32_t) toolParam->getIndex() - 1);
                                                     });
}

int RemoteControlsHelper::getNumPages()
{
    return ToolTypes::count;
}

void RemoteControlsHelper::pageFill (uint32_t pageIndex,
                                     juce::String& sectionName,
                                     uint32_t& pageID,
                                     juce::String& pageName,
                                     std::array<juce::AudioProcessorParameter*, CLAP_REMOTE_CONTROLS_COUNT>& paramPtrs)
{
    if (ToolTypes::IndexOf<dsp::eq::EQProcessor> == (int) pageIndex)
    {
        sectionName = "Processors";
        pageName = "EQ";
        pageID = pageIndex;

        auto& eqParams = params.eqParams->eqParams.eqParams;
        for (size_t i = 0; i < CLAP_REMOTE_CONTROLS_COUNT; ++i)
            paramPtrs[i] = eqParams[i].freqParam.get();
    }
    else if (ToolTypes::IndexOf<dsp::waveshaper::WaveshaperProcessor> == (int) pageIndex)
    {
        sectionName = "Processors";
        pageName = "Waveshaper";
        pageID = pageIndex;

        paramPtrs[0] = params.waveshaperParams->gainParam.get();
        paramPtrs[1] = params.waveshaperParams->shapeParam.get();
        paramPtrs[2] = params.waveshaperParams->oversampleParam.get();
    }
    else if (ToolTypes::IndexOf<dsp::signal_gen::SignalGeneratorProcessor> == (int) pageIndex)
    {
        sectionName = "Processors";
        pageName = "Signal Generator";
        pageID = pageIndex;

        paramPtrs[0] = params.signalGenParams->oscillatorChoice.get();
        paramPtrs[1] = params.signalGenParams->frequency.get();
        paramPtrs[2] = params.signalGenParams->gain.get();
    }
    else if (ToolTypes::IndexOf<dsp::analog_eq::AnalogEQProcessor> == (int) pageIndex)
    {
        sectionName = "Processors";
        pageName = "Analog EQ";
        pageID = pageIndex;

        params.analogEQParams->doForAllParameters ([&paramPtrs] (juce::RangedAudioParameter& param, size_t idx)
                                                   {
                                                       paramPtrs[idx] = &param;
                                                   });
    }
    else if (ToolTypes::IndexOf<dsp::band_splitter::BandSplitterProcessor> == (int) pageIndex)
    {
        sectionName = "Processors";
        pageName = "Band Splitter";
        pageID = pageIndex;

        paramPtrs[0] = params.bandSplitParams->slope.get();
        paramPtrs[1] = params.bandSplitParams->cutoff.get();
        paramPtrs[2] = params.bandSplitParams->cutoff2.get();
        paramPtrs[3] = params.bandSplitParams->threeBandOnOff.get();
    }
    else if (ToolTypes::IndexOf<dsp::brickwall::BrickwallProcessor> == (int) pageIndex)
    {
        sectionName = "Processors";
        pageName = "Brickwall";
        pageID = pageIndex;

        paramPtrs[0] = params.brickwallParams->cutoff.get();
        paramPtrs[1] = params.brickwallParams->order.get();
        paramPtrs[2] = params.brickwallParams->filterType.get();
        paramPtrs[3] = params.brickwallParams->filterMode.get();
    }
    else if (ToolTypes::IndexOf<dsp::svf::SVFProcessor> == (int) pageIndex)
    {
        sectionName = "Processors";
        pageName = "SVF";
        pageID = pageIndex;

        paramPtrs[0] = params.svfParams->cutoff.get();
        paramPtrs[1] = params.svfParams->qParam.get();
        paramPtrs[2] = params.svfParams->type.get();
        paramPtrs[3] = params.svfParams->mode.get();
    }
}
} // namespace state
