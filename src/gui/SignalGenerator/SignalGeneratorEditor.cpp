#include "SignalGeneratorEditor.h"
#include "gui/Shared/Colours.h"

namespace gui::signal_gen
{
SignalGeneratorEditor::SignalGeneratorEditor (State& state, const chowdsp::HostContextProvider& hcp)
    : oscController (state, hcp),
      oscillatorChoice (state, state.params.signalGenParams->oscillatorChoice)
{
    addAndMakeVisible (oscController);
    addAndMakeVisible (oscillatorChoice);

    oscillatorChoice.linesColour = colours::linesColour;
    oscillatorChoice.thumbColour = colours::thumbColour;
    oscillatorChoice.plotColour = colours::plotColour;
}

void SignalGeneratorEditor::paint (juce::Graphics& g)
{
    g.setGradientFill (juce::ColourGradient { colours::backgroundLight,
                                              juce::Point { 0.0f, 0.0f },
                                              colours::backgroundDark,
                                              juce::Point { (float) getWidth() * 0.35f, (float) getHeight() * 0.5f },
                                              false });
    g.fillAll();
}

void SignalGeneratorEditor::resized()
{
    auto bounds = getLocalBounds();
    oscController.setBounds (bounds);
    oscillatorChoice.setBounds (bounds.removeFromBottom (proportionOfHeight (0.075f)));
}
} // namespace gui::signal_gen
