#include "PluginState.h"
#include "dsp/MultiToolProcessor.h"
#include "NameHelpers.h"

namespace state
{
const juce::StringArray PluginParams::toolChoices = []
{
    juce::StringArray choices { "None" };
    chowdsp::ForEachInTypeList<dsp::ToolTypes>::doForEach (
        [&choices] (auto toolTypeIndex)
        {
            using ToolType = typename dsp::ToolTypes::template AtIndex<toolTypeIndex>;
            choices.add (NameHelpers::getNameForProcessor<ToolType>());
        });
    return choices;
}();
}
