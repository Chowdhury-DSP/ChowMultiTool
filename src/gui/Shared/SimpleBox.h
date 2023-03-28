#pragma once

#include "Fonts.h"

namespace gui
{
struct SimpleBox : juce::ComboBox
{
    struct LNF : juce::LookAndFeel_V4
    {
        LNF();
        juce::Font getPopupMenuFont() override;
        gui::SharedFonts fonts;
    };

    SimpleBox();
    ~SimpleBox() override;

    void paint (juce::Graphics& g) override;
    void resized() override {}

    LNF lnf;
    gui::SharedFonts fonts;
    juce::String extraText;
};
} // namespace gui