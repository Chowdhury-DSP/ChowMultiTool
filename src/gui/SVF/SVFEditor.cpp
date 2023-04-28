#include "SVFEditor.h"
#include "BottomBar.h"
#include "gui/Shared/Colours.h"

namespace gui::svf
{
SVFEditor::SVFEditor (State& pluginState, dsp::svf::Params& svfParams, const chowdsp::HostContextProvider& hcp)
    : plot (pluginState, svfParams, hcp),
      paramControls (pluginState, svfParams),
      keytrackButton ("Vector/fad-keyboard.svg", colours::plotColour, colours::linesColour),
      keytrackAttach (svfParams.keytrack, pluginState, keytrackButton),
      arpLimitButton ("Vector/arrows-up-to-line-solid.svg", colours::plotColour, colours::linesColour),
      arpLimitAttach (svfParams.arpLimitMode, pluginState, arpLimitButton)
{
    addAndMakeVisible (plot);
    addAndMakeVisible (paramControls);

    bottomBar = std::make_unique<BottomBar> (pluginState, svfParams);
    addAndMakeVisible (bottomBar.get());

    addAndMakeVisible (keytrackButton);
    addChildComponent (arpLimitButton);
    arpLimitButton.setVisible (svfParams.type->get() == dsp::svf::SVFType::ARP);
    modeChangeCallback = pluginState.addParameterListener (svfParams.type,
                                                           chowdsp::ParameterListenerThread::MessageThread,
                                                           [this, &svfParams]
                                                           {
                                                               arpLimitButton.setVisible (svfParams.type->get() == dsp::svf::SVFType::ARP);
                                                           });
}

void SVFEditor::paint (juce::Graphics& g)
{
    g.setGradientFill (juce::ColourGradient { colours::backgroundLight,
                                              juce::Point { 0.0f, 0.0f },
                                              colours::backgroundDark,
                                              juce::Point { (float) getWidth() * 0.35f, (float) getHeight() * 0.5f },
                                              false });
    g.fillAll();
}

void SVFEditor::resized()
{
    auto bounds = getLocalBounds();
    bottomBar->setBounds (bounds.removeFromBottom (proportionOfHeight (0.075f)));
    paramControls.setBounds (bounds.removeFromRight (proportionOfWidth (0.15f)));
    plot.setBounds (bounds);

    const auto pad = proportionOfWidth (0.005f);
    const auto buttonDim = proportionOfWidth (0.035f);
    keytrackButton.setBounds (bounds.getWidth() - pad - buttonDim, pad, buttonDim, buttonDim);
    arpLimitButton.setBounds (bounds.getWidth() - 2 * (pad + buttonDim), pad, buttonDim, buttonDim);
}
} // namespace gui::svf
