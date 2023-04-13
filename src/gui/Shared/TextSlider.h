#pragma once

#include "Fonts.h"

namespace gui
{
class TextSlider : public juce::Slider
{
public:
    TextSlider (chowdsp::PluginState& state, chowdsp::FloatParameter* param);
    ~TextSlider() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;

private:
    bool multiClicking = false;

    const chowdsp::FloatParameter* parameter;
    chowdsp::SliderAttachment attachment;

    chowdsp::SharedLNFAllocator lnfAllocator;
    SharedFonts fonts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TextSlider)
};
} // namespace gui
