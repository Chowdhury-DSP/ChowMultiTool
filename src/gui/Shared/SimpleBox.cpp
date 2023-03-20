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

SimpleBox::SimpleBox()
{
    setLookAndFeel (lnfs->getLookAndFeel<LNF>());
}
SimpleBox::~SimpleBox()
{
    setLookAndFeel (nullptr);
}

void SimpleBox::paint (juce::Graphics& g)
{
    g.setFont (juce::Font (fonts->robotoBold).withHeight (0.6f * (float) getHeight()));
    g.setColour (findColour (juce::ComboBox::textColourId));
    g.drawFittedText (extraText + getText(), getLocalBounds(), juce::Justification::centred, 1);
}
} // namespace gui
