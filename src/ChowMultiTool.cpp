#include "ChowMultiTool.h"
#include "gui/PluginEditor.h"

namespace
{
const juce::String settingsFilePath = "ChowdhuryDSP/ChowMultiTool/.plugin_settings.json";
const juce::String logFileSubDir = "ChowdhuryDSP/ChowMultiTool/Logs";
const juce::String logFileNameRoot = "ChowMultiTool_Log_";
} // namespace

ChowMultiTool::ChowMultiTool() : chowdsp::PluginBase<State> (&undoManager),
                                 logger (logFileSubDir, logFileNameRoot),
                                 processor (state.params)
{
    juce::Logger::writeToLog (chowdsp::PluginDiagnosticInfo::getDiagnosticsString (*this));
    pluginSettings->initialise (settingsFilePath);
}

void ChowMultiTool::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    processor.prepare ({ sampleRate, (uint32_t) samplesPerBlock, (uint32_t) getMainBusNumInputChannels() });
}

void ChowMultiTool::processAudioBlock (juce::AudioBuffer<float>& buffer)
{
    processor.processBlock (buffer);

    chowdsp::BufferMath::sanitizeBuffer (buffer);
}

juce::AudioProcessorEditor* ChowMultiTool::createEditor()
{
    return new gui::PluginEditor { *this };
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChowMultiTool();
}
