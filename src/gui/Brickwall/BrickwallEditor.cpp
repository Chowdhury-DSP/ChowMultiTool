#include "BrickwallEditor.h"
#include "BottomBar.h"
#include "gui/Shared/Colours.h"

namespace gui::brickwall
{
BrickwallEditor::BrickwallEditor (State& pluginState,
                                  dsp::brickwall::Params& params,
                                  dsp::brickwall::ExtraState& brickwallExtraState,
                                  const chowdsp::HostContextProvider& hcp,
                                  std::pair<gui::SpectrumAnalyserTask::Optional, gui::SpectrumAnalyserTask::Optional> spectrumAnalyserTasks)
    : plot (pluginState, params, brickwallExtraState, hcp, spectrumAnalyserTasks)
{
    bottomBar = std::make_unique<BottomBar> (pluginState, params);

    addAndMakeVisible (plot);
    addAndMakeVisible (bottomBar.get());
}

BrickwallEditor::~BrickwallEditor() {};

void BrickwallEditor::paint (juce::Graphics& g)
{
    g.setGradientFill (juce::ColourGradient { colours::backgroundLight,
                                              juce::Point { 0.0f, 0.0f },
                                              colours::backgroundDark,
                                              juce::Point { (float) getWidth() * 0.35f, (float) getHeight() * 0.5f },
                                              false });
    g.fillAll();
}

void BrickwallEditor::resized()
{
    auto bounds = getLocalBounds();
    plot.setBounds (bounds);
    bottomBar->setBounds (bounds.removeFromBottom (proportionOfHeight (0.075f)));
}
} // namespace gui::brickwall
