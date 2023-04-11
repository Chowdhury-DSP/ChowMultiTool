#pragma once

#include "Fonts.h"

namespace gui
{
struct SimpleBox : juce::ComboBox
{
    struct LNF : chowdsp::ChowLNF
    {
        LNF();
        juce::Font getPopupMenuFont() override;
        juce::PopupMenu::Options getOptionsForComboBoxPopupMenu (juce::ComboBox&, juce::Label&) override;
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