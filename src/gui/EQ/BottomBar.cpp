#include "BottomBar.h"
#include "gui/Shared/Colours.h"

namespace gui::eq
{
BottomBar::BottomBar (State& pluginState, dsp::eq::EQToolParams& params)
{
    for (auto [idx, box] : chowdsp::enumerate (boxes))
    {
        addAndMakeVisible (box);
        box.initialise (pluginState, params.eqParams.eqParams[idx].onOffParam, params.eqParams.eqParams[idx].typeParam);
        box.getLookAndFeel().setColour (juce::PopupMenu::highlightedBackgroundColourId, colours::thumbColours[idx].withAlpha (0.75f));
    }
}

void BottomBar::resized()
{
    auto bounds = getLocalBounds();
    const auto fracWidth = proportionOfWidth (1.0f / (float) boxes.size());
    for (auto& box : boxes)
        box.setBounds (bounds.removeFromLeft (fracWidth));
}

void BottomBar::paint (juce::Graphics& g)
{
    const auto verticalGrad = [this] (juce::Colour top, juce::Colour bottom)
    {
        return juce::ColourGradient::vertical (top, 0.0f, bottom, (float) getHeight());
    };

    g.setGradientFill (verticalGrad (juce::Colours::black.withAlpha (0.0f), juce::Colours::black));
    g.fillAll();

    const auto fracWidthPos = (float) getWidth() / (float) dsp::eq::EQToolParams::numBands;
    for (size_t i = 0; i < dsp::eq::EQToolParams::numBands; ++i)
    {
        const auto startX = float (i) * fracWidthPos;
        const auto endX = float (i + 1) * fracWidthPos;

        g.setGradientFill (verticalGrad (colours::thumbColours[i].withAlpha (0.0f), colours::thumbColours[i]));
        g.fillRect (juce::Rectangle<float> { startX, 0.0f, endX - startX, (float) getHeight() });

        if (i < (dsp::eq::EQToolParams::numBands - 1))
        {
            g.setGradientFill (verticalGrad (colours::linesColour.withAlpha (0.75f), colours::linesColour));
            g.drawLine (juce::Line { juce::Point { endX, 0.0f }, juce::Point { endX, (float) getHeight() } }, 1.0f);
        }
    }
}
} // namespace gui::eq
