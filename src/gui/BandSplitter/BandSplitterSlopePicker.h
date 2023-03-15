#pragma once

#include "dsp/BandSplitter/BandSplitterProcessor.h"
#include "state/PluginState.h"

namespace gui::band_splitter
{
class SlopePicker : public juce::Component
{
public:
    SlopePicker() = default;

    void paint (juce::Graphics& g) override
    {
        g.setGradientFill (juce::ColourGradient {
            juce::Colours::black.withAlpha (0.0f),
            juce::Point { 0.0f, 0.0f },
            juce::Colours::black,
            juce::Point { 0.0f, (float) getHeight() },
            false
        });
        g.fillAll();

        g.setGradientFill (juce::ColourGradient {
            juce::Colour { 0xFF3399BB }.withAlpha (0.05f),
            juce::Point { 0.0f, 0.0f },
            juce::Colour { 0xFF3399BB },
            juce::Point { 0.0f, (float) getHeight() },
            false
        });
        g.fillAll();

        g.setColour (juce::Colour { 0xFFD3D3D3 });
        for (auto pct : { 0.2f, 0.4f, 0.6f, 0.8f })
        {
            const auto xPos = (float) getWidth() * pct;
            g.drawLine (juce::Line { juce::Point { xPos, 0.0f },
                        juce::Point { xPos, (float) getHeight() } },
                        1.0f);
        }
    }
};
}
