#pragma once

#include "state/PluginState.h"
#include "WaveshaperColours.h"

namespace gui::waveshaper
{
class BottomBar : public juce::Component
{
public:
    BottomBar (State& pluginState, dsp::waveshaper::Params& wsParams);

    void resized() override;
    void paint (juce::Graphics& g) override;

private:
    struct SimpleBox : juce::ComboBox
    {
        void paint (juce::Graphics& g) override
        {
            g.setFont (juce::Font { juce::String { "Roboto" }, 0.6f * (float) getHeight(), juce::Font::FontStyleFlags::bold });
            g.setColour (colours::linesColour);
            g.drawFittedText (getText(), getLocalBounds(), juce::Justification::centred, 1);
        }
        void resized() override {}
    };

    SimpleBox shapeMenu;
    SimpleBox oversampleMenu;

    chowdsp::ComboBoxAttachment shapeAttach;
    chowdsp::ComboBoxAttachment osAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BottomBar)
};
}
