#pragma once

#include "OscillatorController.h"
#include "state/PluginState.h"

namespace gui::signal_gen
{
class SignalGeneratorEditor : public juce::Component
{
public:
    explicit SignalGeneratorEditor (State& state);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    juce::ComboBox oscillatorChoiceBox;
    chowdsp::ComboBoxAttachment oscillatorChoiceAttachment;

    OscillatorController oscController;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SignalGeneratorEditor)
};
} // namespace gui::signal_gen
