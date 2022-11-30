#pragma once

#include <pch.h>

namespace gui::waveshaper
{
class WaveshaperEditor : public juce::Component
{
public:
    WaveshaperEditor() = default;

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::blue);
    }
};
} // namespace gui::waveshaper
