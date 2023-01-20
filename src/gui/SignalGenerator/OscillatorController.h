#pragma once

#include "state/PluginState.h"
#include "OscillatorPlot.h"

namespace gui::signal_gen
{
class OscillatorController : public juce::Component
{
public:
    explicit OscillatorController (State& state);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    chowdsp::OscillatorPlot plot;

    dsp::signal_gen::SignalGeneratorProcessor plotSignalGen;

    std::unique_ptr<juce::Slider> freqSlider;
    std::unique_ptr<juce::Slider> gainSlider;

    chowdsp::ScopedCallbackList parameterChangeListeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscillatorController)
};
}
