#pragma once

#include "dsp/MultiToolProcessor.h"
#include "state/PluginRemoteControls.h"
#include "state/PluginState.h"

class ChowMultiTool : public chowdsp::PluginBase<State>
{
public:
    ChowMultiTool();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processAudioBlock (juce::AudioBuffer<float>& buffer) override;
    bool isBusesLayoutSupported (const BusesLayout& layout) const override;

    juce::AudioProcessorEditor* createEditor() override;

    // CLAP extensions
    bool supportsRemoteControls() const noexcept override { return true; }
    uint32_t remoteControlsPageCount() noexcept override { return (uint32_t) remoteControls.getNumPages(); }
    bool remoteControlsPageFill (uint32_t pageIndex,
                                 juce::String& sectionName,
                                 uint32_t& pageID,
                                 juce::String& pageName,
                                 std::array<juce::AudioProcessorParameter*, CLAP_REMOTE_CONTROLS_COUNT>& params) noexcept override;

private:
    static BusesProperties createBusLayout();

    chowdsp::PluginLogger logger;
    chowdsp::SharedPluginSettings pluginSettings;

    dsp::MultiToolProcessor processor { *this, state };
    state::RemoteControlsHelper remoteControls { state, state.params, this };

    juce::UndoManager undoManager { 500000 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChowMultiTool)
};
