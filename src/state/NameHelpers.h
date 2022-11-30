#pragma once

#include <pch.h>

namespace NameHelpers
{
juce::String undoCamelCase (const juce::String& str)
{
    for (int i = str.length() - 1; i >= 1; --i)
    {
        using CharFunctions = juce::CharacterFunctions;
        if (CharFunctions::isUpperCase (str[i]) && CharFunctions::isLowerCase (str[i-1]))
            return undoCamelCase (undoCamelCase (str.substring (0, i)) + " " + str.substring (i));
    }

    return str;
}

template <typename ProcessorType>
juce::String getNameForProcessor()
{
    static constexpr std::string_view nameSuffix = "Processor";
    static constexpr auto toolName = chowdsp::NameOfHelpers::getLocalTypeName<ProcessorType>();
    static constexpr auto toolDisplayName = toolName.substr (0, toolName.size() - nameSuffix.size());

    return undoCamelCase (juce::String { toolDisplayName.data(), toolDisplayName.size() });
}
}
