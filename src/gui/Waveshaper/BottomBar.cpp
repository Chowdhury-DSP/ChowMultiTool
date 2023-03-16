#include "BottomBar.h"
#include "dsp/Waveshaper/WaveshaperProcessor.h"

namespace gui::waveshaper
{
BottomBar::BottomBar (State& pluginState, dsp::waveshaper::Params& wsParams)
    : shapeAttach (wsParams.shapeParam, pluginState, shapeMenu),
      osAttach (wsParams.oversampleParam, pluginState, oversampleMenu)
{
    addAndMakeVisible (shapeMenu);
    addAndMakeVisible (oversampleMenu);
}

void BottomBar::paint (juce::Graphics& g)
{
    const auto verticalGrad = [this] (juce::Colour top, juce::Colour bottom)
    {
        return juce::ColourGradient::vertical (top, 0.0f, bottom, (float) getHeight());
    };

    auto backgroundGrad = verticalGrad (colours::boxColour.withAlpha (0.0f), colours::boxColour);
    backgroundGrad.addColour (0.5, colours::boxColour.withAlpha (0.25f));
    g.setGradientFill (backgroundGrad);
    g.fillAll();

    g.setColour (colours::linesColour);
    const auto halfWidthPos = (float) getWidth() * 0.5f;
    g.drawLine (juce::Line { juce::Point { halfWidthPos, 0.0f }, juce::Point { halfWidthPos, (float) getHeight() } }, 1.0f);
}

void BottomBar::resized()
{
    auto bounds = getLocalBounds();
    shapeMenu.setBounds (bounds.removeFromLeft (proportionOfWidth (0.5f)));
    oversampleMenu.setBounds (bounds);
}
} // namespace gui::waveshaper
