#pragma once

#include "OscillatorPlot.h"
#include "dsp/SignalGenerator/SignalGeneratorProcessor.h"
#include "state/PluginState.h"
#include "gui/Shared/DotSlider.h"

namespace gui::signal_gen
{
class OscillatorController : public juce::Component
{
public:
    explicit OscillatorController (State& state);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    OscillatorPlot plot;

    dsp::signal_gen::SignalGeneratorProcessor plotSignalGen;

    DotSliderGroup sliders;

    chowdsp::ScopedCallbackList parameterChangeListeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscillatorController)
};
} // namespace gui::signal_gen
