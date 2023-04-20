#include "InitPage.h"
#include "gui/Shared/Colours.h"
#include "ChowMultiTool.h"

namespace gui::init
{
InitPage::InitPage (const ChowMultiTool& plug) : plugin (plug)
{
    const auto fs = cmrc::gui::get_filesystem();
    const auto logoSvg = fs.open ("logo.svg");
    logo = juce::Drawable::createFromImageData (logoSvg.begin(), logoSvg.size());

    linkButton.setColour (juce::HyperlinkButton::textColourId, colours::linesColour);
    addAndMakeVisible (linkButton);
}

void InitPage::paint (juce::Graphics& g)
{
    g.setGradientFill (juce::ColourGradient { colours::backgroundLight,
                                              juce::Point { 0.0f, 0.0f },
                                              colours::backgroundDark,
                                              juce::Point { (float) getWidth() * 0.35f, (float) getHeight() * 0.5f },
                                              false });
    g.fillAll();

    const auto pad = proportionOfHeight (0.05f);
    auto textBounds = juce::Rectangle { pad, pad, proportionOfWidth (0.4375f), proportionOfHeight (0.25f) };

    const auto font = juce::Font { fonts->robotoBold }.withHeight ((float) textBounds.getHeight());
    g.setFont (font);

    g.setColour (colours::linesColour);
    g.drawFittedText ("Chow", textBounds, juce::Justification::centredLeft, 1);

    textBounds = textBounds.withY (proportionOfHeight (0.3f)).withWidth (font.getStringWidth ("Multi"));
    g.setColour (logo::colours::backgroundBlue);
    g.drawFittedText ("Multi", textBounds, juce::Justification::centredLeft, 1);

    textBounds = textBounds.withX (textBounds.getRight()).withWidth (font.getStringWidth ("Tool"));
    g.setColour (logo::colours::accentRed);
    g.drawFittedText ("Tool", textBounds, juce::Justification::centredLeft, 1);

    const auto logoDim = proportionOfWidth (0.33333f);
    auto logoBounds = juce::Rectangle { getWidth() - logoDim - pad, pad, logoDim, logoDim };
    logo->drawWithin (g, logoBounds.toFloat(), juce::RectanglePlacement::fillDestination, 1.0f);

    const auto smallTextHeight = proportionOfHeight (0.075f);
    const auto smallFont = juce::Font { fonts->robotoBold }.withHeight ((float) smallTextHeight);
    g.setFont (smallFont);
    g.setColour (colours::linesColour);

    using Info = chowdsp::StandardInfoProvider;

    auto smallTextBounds = juce::Rectangle { 0, proportionOfHeight (0.65f), getWidth(), smallTextHeight };
    g.drawFittedText (Info::getVersionString(), smallTextBounds, juce::Justification::centred, 1);

    smallTextBounds = smallTextBounds.withY (proportionOfHeight (0.75f));
    g.drawFittedText (Info::getPlatformString() + ", " + Info::getWrapperTypeString (plugin), smallTextBounds, juce::Justification::centred, 1);
}

void InitPage::resized()
{
    const auto smallTextHeight = proportionOfHeight (0.075f);
    const auto smallFont = juce::Font { fonts->robotoBold }.withHeight ((float) smallTextHeight);
    linkButton.setFont (smallFont, false, juce::Justification::centred);
    linkButton.setBounds (juce::Rectangle { 0, proportionOfHeight (0.85f), getWidth(), smallTextHeight });
}
} // namespace gui::init
