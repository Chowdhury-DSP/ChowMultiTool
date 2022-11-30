#pragma once

#include <pch.h>

namespace gui::eq
{
class EQEditor : public juce::Component
{
public:
    EQEditor() = default;

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::red);
    }
};
} // namespace gui::eq
