#include "SimpleBox.h"
#include "Colours.h"

namespace gui
{
SimpleBox::LNF::LNF()
{
    setColour (juce::ComboBox::textColourId, colours::linesColour);
    setColour (juce::PopupMenu::backgroundColourId, colours::backgroundDark);
    setColour (juce::PopupMenu::textColourId, colours::linesColour);
    setColour (juce::PopupMenu::highlightedTextColourId, colours::linesColour);
}

juce::Font SimpleBox::LNF::getPopupMenuFont()
{
    return fonts->robotoBold;
}

juce::PopupMenu::Options SimpleBox::LNF::getOptionsForComboBoxPopupMenu (juce::ComboBox& box, juce::Label& label)
{
    return chowdsp::ChowLNF::getOptionsForComboBoxPopupMenu (box, label)
#if JUCE_IOS
        .withItemThatMustBeVisible (box.getNumItems() / 2)
#endif
        .withStandardItemHeight (box.getHeight());
}

SimpleBox::SimpleBox()
{
    setLookAndFeel (&lnf);
}
SimpleBox::~SimpleBox()
{
    setLookAndFeel (nullptr);
}

void SimpleBox::paint (juce::Graphics& g)
{
    g.setFont (juce::Font (fonts->robotoBold).withHeight (0.6f * (float) getHeight()));
    g.setColour (findColour (juce::ComboBox::textColourId));
    g.drawFittedText (extraText + getText(), getLocalBounds().reduced (proportionOfWidth (0.05f), 0), juce::Justification::centred, 1);
}
} // namespace gui
