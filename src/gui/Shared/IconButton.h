#pragma once

#include <pch.h>

namespace gui
{
struct IconButton : juce::Button
{
    explicit IconButton (const std::string& iconTag,
                         juce::Colour onColour,
                         juce::Colour offColour)
        : juce::Button ({})
    {
        const auto fs = cmrc::gui::get_filesystem();
        const auto svg = fs.open (iconTag);
        const auto icon = juce::Drawable::createFromImageData (svg.begin(), svg.size());
        iconOn = icon->createCopy();
        iconOn->replaceColour (juce::Colours::black, onColour);
        iconOff = icon->createCopy();
        iconOff->replaceColour (juce::Colours::black, offColour);

        setClickingTogglesState (true);
    }

    void paintButton (juce::Graphics& g, bool, bool) override
    {
        g.setColour (juce::Colours::black.withAlpha (0.75f));
        g.fillRoundedRectangle (getLocalBounds().toFloat(), 0.1f * (float) getHeight());

        const auto pad = proportionOfWidth (0.2f);
        const auto& icon = getToggleState() ? iconOn : iconOff;
        icon->drawWithin (g, getLocalBounds().reduced (pad).toFloat(), juce::RectanglePlacement::stretchToFit, 1.0f);
    }

    std::unique_ptr<juce::Drawable> iconOn;
    std::unique_ptr<juce::Drawable> iconOff;
};
}
