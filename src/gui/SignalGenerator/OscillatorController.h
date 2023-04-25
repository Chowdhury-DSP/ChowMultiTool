#pragma once

#include "OscillatorPlot.h"
#include "dsp/SignalGenerator/SignalGeneratorProcessor.h"
#include "gui/Shared/DotSlider.h"
#include "state/PluginState.h"
#include "SignalGenChyron.h"

namespace gui::signal_gen
{
class OscillatorController : public juce::Component
{
public:
    OscillatorController (State& state, const chowdsp::HostContextProvider& hcp);

    void resized() override;

private:
    OscillatorPlot plot;

    SpectrumDotSlider gainSlider, freqSlider;
    DotSliderGroup sliders;
    SignalGenChyron chyron;

    chowdsp::ScopedCallbackList parameterChangeListeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscillatorController)
};
} // namespace gui::signal_gen
