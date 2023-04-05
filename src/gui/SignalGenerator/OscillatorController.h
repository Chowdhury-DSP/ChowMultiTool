#pragma once

#include "OscillatorPlot.h"
#include "dsp/SignalGenerator/SignalGeneratorProcessor.h"
#include "gui/Shared/DotSlider.h"
#include "state/PluginState.h"

namespace gui::signal_gen
{
class OscillatorController : public juce::Component
{
public:
    explicit OscillatorController (State& state);

    void resized() override;

private:
    OscillatorPlot plot;

    dsp::signal_gen::SignalGeneratorProcessor plotSignalGen;

    SpectrumDotSlider gainSlider, freqSlider;
    DotSliderGroup sliders;

    chowdsp::ScopedCallbackList parameterChangeListeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscillatorController)
};
} // namespace gui::signal_gen
