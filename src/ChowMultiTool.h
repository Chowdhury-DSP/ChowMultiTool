#pragma once

#include "state/PluginState.h"
#include "dsp/MultiToolProcessor.h"

class ChowMultiTool : public chowdsp::PluginBase<State>
{
public:
    ChowMultiTool();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processAudioBlock (juce::AudioBuffer<float>& buffer) override;
    bool isBusesLayoutSupported (const BusesLayout& layout) const override;

    juce::AudioProcessorEditor* createEditor() override;

private:
    static BusesProperties createBusLayout();

    chowdsp::PluginLogger logger;
    chowdsp::SharedPluginSettings pluginSettings;

    dsp::MultiToolProcessor processor;

    juce::UndoManager undoManager { 500000 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChowMultiTool)
};
