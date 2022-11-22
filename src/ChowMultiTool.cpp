#include "ChowMultiTool.h"

ChowMultiTool::ChowMultiTool()
{
}

void ChowMultiTool::addParameters (Parameters& params)
{
}

void ChowMultiTool::prepareToPlay (double sampleRate, int samplesPerBlock)
{
}

void ChowMultiTool::processAudioBlock (AudioBuffer<float>& buffer)
{
}

AudioProcessorEditor* ChowMultiTool::createEditor()
{
    return new GenericAudioProcessorEditor { *this };
}

// This creates new instances of the plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChowMultiTool();
}
