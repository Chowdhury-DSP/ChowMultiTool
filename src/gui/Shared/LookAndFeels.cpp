#include "LookAndFeels.h"
#include "Colours.h"

namespace gui::lnf
{
MenuLNF::MenuLNF()
{
    setColour (juce::PopupMenu::ColourIds::highlightedBackgroundColourId, logo::colours::backgroundBlue);
}

void MenuLNF::drawPopupMenuItem (juce::Graphics& g,
                                 const juce::Rectangle<int>& area,
                                 const bool isSeparator,
                                 const bool isActive,
                                 const bool isHighlighted,
                                 const bool /*isTicked*/,
                                 const bool hasSubMenu,
                                 const juce::String& text,
                                 const juce::String& shortcutKeyText,
                                 const juce::Drawable* icon,
                                 const juce::Colour* const textColourToUse)
{
    juce::LookAndFeel_V4::drawPopupMenuItem (g, area, isSeparator, isActive, isHighlighted, false /*isTicked*/, hasSubMenu, text, shortcutKeyText, icon, textColourToUse);
}

void MenuLNF::drawPopupMenuBackground (juce::Graphics& g, [[maybe_unused]] int width, [[maybe_unused]] int height)
{
    g.fillAll (colours::backgroundLight);
}
} // namespace gui::lnf
