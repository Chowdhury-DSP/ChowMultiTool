#pragma once

#include "OscillatorController.h"
#include "gui/Shared/ChoicePicker.h"
#include "state/PluginState.h"

namespace gui::signal_gen
{
class SignalGeneratorEditor : public juce::Component
{
public:
    SignalGeneratorEditor (State& state, const chowdsp::HostContextProvider& hcp);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    OscillatorController oscController;
    ChoicePicker<dsp::signal_gen::Oscillator> oscillatorChoice;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SignalGeneratorEditor)
};
} // namespace gui::signal_gen
