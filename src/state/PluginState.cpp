#include "PluginState.h"
#include "NameHelpers.h"
#include "dsp/MultiToolProcessor.h"

namespace state
{
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
}
