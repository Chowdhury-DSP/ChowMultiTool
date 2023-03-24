#include "EQEditor.h"
#include "BottomBar.h"
#include "gui/Shared/Colours.h"

namespace gui::eq
{
EQEditor::EQEditor (State& pluginState, dsp::eq::EQToolParams& eqParams)
    : plot (pluginState, eqParams.eqParams),
      paramsView (pluginState, eqParams)
{
    bottomBar = std::make_unique<BottomBar> (pluginState, eqParams);

    addAndMakeVisible (plot);
    addAndMakeVisible (bottomBar.get());
    //    addAndMakeVisible (paramsView);
}

void EQEditor::paint (juce::Graphics& g)
{
    g.setGradientFill (juce::ColourGradient { colours::backgroundLight,
                                              juce::Point { 0.0f, 0.0f },
                                              colours::backgroundDark,
                                              juce::Point { (float) getWidth() * 0.35f, (float) getHeight() * 0.5f },
                                              false });
    g.fillAll();
}

void EQEditor::resized()
{
    auto bounds = getLocalBounds();
    plot.setBounds (bounds);
    bottomBar->setBounds (bounds.removeFromBottom (proportionOfHeight (0.075f)));
}
} // namespace gui::eq
