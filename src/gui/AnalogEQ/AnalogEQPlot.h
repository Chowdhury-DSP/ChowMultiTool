#pragma once

#include "EQChyron.h"
#include "dsp/AnalogEQ/AnalogEQProcessor.h"
#include "state/PluginState.h"

namespace gui::analog_eq
{
class DotSlider;

class AnalogEQPlot : public chowdsp::SpectrumPlotBase
{
public:
    AnalogEQPlot (State& pluginState, dsp::analog_eq::Params& params, const chowdsp::HostContextProvider& hcp);
    ~AnalogEQPlot() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void mouseDown (const juce::MouseEvent& e) override;

    void updatePlot();

    chowdsp::GenericFilterPlotter filterPlotter;
    dsp::analog_eq::AnalogEQProcessor pultecEQ;

    chowdsp::ScopedCallbackList callbacks;

    std::unique_ptr<juce::Slider> lowFreqControl;
    std::unique_ptr<juce::Slider> lowBoostControl;
    std::unique_ptr<juce::Slider> lowCutControl;
    std::unique_ptr<juce::Slider> highBoostControl;
    std::unique_ptr<juce::Slider> highCutControl;
    std::unique_ptr<juce::Slider> highBoostFreqControl;
    std::unique_ptr<juce::Slider> highCutFreqControl;
    std::unique_ptr<juce::Component> highBoostFullControl;
    std::unique_ptr<juce::Component> highCutFullControl;

    EQChyron chyron;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalogEQPlot)
};
} // namespace gui::analog_eq
