#include "BottomBar.h"
#include "dsp/Brickwall/BrickwallProcessor.h"
#include "gui/Shared/Colours.h"

namespace gui::brickwall
{
BottomBar::BottomBar (State& pluginState, dsp::brickwall::Params& params)
    : modeAttach (params.filterMode, pluginState, modeMenu),
      filterTypeAttach (params.filterType, pluginState, filterTypeMenu),
      slopeAttach (params.order, pluginState, slopeMenu)
{
    for (auto* menu : { &modeMenu, &filterTypeMenu, &slopeMenu })
    {
        addAndMakeVisible (*menu);
        menu->getLookAndFeel().setColour (juce::PopupMenu::highlightedBackgroundColourId, colours::thumbColour.withAlpha (0.75f));
    }
}

void BottomBar::paint (juce::Graphics& g)
{
    const auto verticalGrad = [this] (juce::Colour top, juce::Colour bottom)
    {
        return juce::ColourGradient::vertical (top, 0.0f, bottom, (float) getHeight());
    };

    g.setGradientFill (verticalGrad (juce::Colours::black.withAlpha (0.0f), juce::Colours::black));
    g.fillAll();

    g.setGradientFill (verticalGrad (colours::thumbColour.withAlpha (0.0f), colours::thumbColour));
    g.fillAll();

    g.setGradientFill (verticalGrad (colours::linesColour.withAlpha (0.75f), colours::linesColour));
    const auto thirdWidthPos = (float) getWidth() / 3.0f;
    g.drawLine (juce::Line { juce::Point { thirdWidthPos, 0.0f }, juce::Point { thirdWidthPos, (float) getHeight() } }, 1.0f);
    g.drawLine (juce::Line { juce::Point { 2.0f * thirdWidthPos, 0.0f }, juce::Point { 2.0f * thirdWidthPos, (float) getHeight() } }, 1.0f);
}

void BottomBar::resized()
{
    auto bounds = getLocalBounds();
    const auto oneThirdWidth = proportionOfWidth (1.0f / 3.0f);
    modeMenu.setBounds (bounds.removeFromLeft (oneThirdWidth));
    filterTypeMenu.setBounds (bounds.removeFromLeft (oneThirdWidth));
    slopeMenu.setBounds (bounds);
}
}
