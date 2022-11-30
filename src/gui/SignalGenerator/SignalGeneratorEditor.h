#pragma once

#include <pch.h>

namespace gui::signal_gen
{
class SignalGeneratorEditor : public juce::Component
{
public:
    SignalGeneratorEditor() = default;

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::green);
    }
};
} // namespace gui::signal_gen
