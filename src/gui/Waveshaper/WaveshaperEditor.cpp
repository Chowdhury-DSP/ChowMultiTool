#include "WaveshaperEditor.h"
#include "BottomBar.h"
#include "gui/Shared/Colours.h"

namespace gui::waveshaper
{
WaveshaperEditor::WaveshaperEditor (State& pluginState, dsp::waveshaper::Params& wsParams)
    : params (wsParams),
      plot (pluginState, wsParams),
      foldFuzzControls (pluginState, wsParams)
{
    bottomBar = std::make_unique<BottomBar>(pluginState, wsParams);

    addAndMakeVisible (plot);
    addAndMakeVisible (bottomBar.get());
    addAndMakeVisible (foldFuzzControls);
}

void WaveshaperEditor::resized()
{
    auto bounds = getLocalBounds();
    bottomBar->setBounds (bounds.removeFromBottom (proportionOfHeight (0.075f)));

    if (params.shapeParam->get() == dsp::waveshaper::Shapes::Fold_Fuzz)
        foldFuzzControls.setBounds (bounds.removeFromRight (proportionOfWidth (0.15f)));

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
