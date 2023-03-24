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

    g.setGradientFill (verticalGrad (colours::thumbColour.withAlpha (0.0f), colours::thumbColour));
    g.fillAll();

    g.setGradientFill (verticalGrad (colours::linesColour.withAlpha (0.75f), colours::linesColour));
    const auto fracWidthPos = (float) getWidth() / (float) dsp::eq::EQToolParams::numBands;
    for (size_t i = 1; i < dsp::eq::EQToolParams::numBands; ++i)
        g.drawLine (juce::Line { juce::Point { (float) i * fracWidthPos, 0.0f }, juce::Point { (float) i * fracWidthPos, (float) getHeight() } }, 1.0f);
}
} // namespace gui::eq
