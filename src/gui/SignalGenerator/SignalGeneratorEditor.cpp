#include "SignalGeneratorEditor.h"

namespace gui::signal_gen
{
SignalGeneratorEditor::SignalGeneratorEditor (State& state)
    : oscillatorChoiceAttachment (*state.params.signalGenParams->oscillatorChoice,
                                  state,
                                  oscillatorChoiceBox),
      oscController (state)
{
    addAndMakeVisible (oscillatorChoiceBox);
    addAndMakeVisible (oscController);
}

void SignalGeneratorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::green);
}

void SignalGeneratorEditor::resized()
{
    auto bounds = getLocalBounds();
    oscController.setBounds (bounds.removeFromTop (proportionOfHeight (0.85f)).reduced (10));
    oscillatorChoiceBox.setBounds (bounds.reduced (20, 0));
}
}
