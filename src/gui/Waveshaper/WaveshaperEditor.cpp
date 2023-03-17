#include "WaveshaperEditor.h"
#include "gui/Shared/Colours.h"

namespace gui::waveshaper
{
WaveshaperEditor::WaveshaperEditor (State& pluginState, dsp::waveshaper::Params& wsParams)
    : plot (pluginState, wsParams),
      bottomBar (pluginState, wsParams),
      paramsView (pluginState, wsParams)
{
    addAndMakeVisible (plot);
    addAndMakeVisible (bottomBar);
    addAndMakeVisible (paramsView);
}

void WaveshaperEditor::resized()
{
    auto bounds = getLocalBounds();
    paramsView.setBounds (bounds.removeFromBottom (proportionOfHeight (0.15f)));
    bottomBar.setBounds (bounds.removeFromBottom (proportionOfHeight (0.075f)));
    plot.setBounds (bounds);
}

void WaveshaperEditor::paint (juce::Graphics& g)
{
    g.setGradientFill (juce::ColourGradient { colours::backgroundLight,
                                              juce::Point { 0.0f, 0.0f },
                                              colours::backgroundDark,
                                              juce::Point { (float) getWidth() * 0.35f, (float) getHeight() * 0.5f },
                                              false });
    g.fillAll();
}
} // namespace gui::waveshaper
