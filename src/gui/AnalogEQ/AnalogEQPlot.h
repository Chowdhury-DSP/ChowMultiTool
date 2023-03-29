#pragma once

#include "dsp/AnalogEQ/AnalogEQProcessor.h"
#include "state/PluginState.h"

namespace gui::analog_eq
{
class DotSlider;

class AnalogEQPlot : public chowdsp::SpectrumPlotBase
{
public:
    AnalogEQPlot (State& pluginState, dsp::analog_eq::Params& params);
    ~AnalogEQPlot() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void updatePlot();

    chowdsp::GenericFilterPlotter filterPlotter;
    dsp::analog_eq::AnalogEQProcessor pultecEQ;

    chowdsp::ScopedCallbackList callbacks;

    std::unique_ptr<juce::Slider> lowFreqControl;
    std::unique_ptr<juce::Slider> lowBoostControl;
    std::unique_ptr<juce::Slider> lowCutControl;
    std::unique_ptr<juce::Slider> highBoostControl;
    std::unique_ptr<juce::Slider> highCutControl;

    //    struct Dot : juce::Component
    //    {
    //        void paint (juce::Graphics& g) override
    //        {
    //            g.setColour (juce::Colours::red);
    //            g.fillEllipse (getLocalBounds().toFloat());
    //        }
    //
    //        std::function<void()> reposition;
    //    };
    //
    //    Dot lowFreqDot;
    //    Dot highBoostFreqDot;
    //    Dot highCutFreqDot;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalogEQPlot)
};
} // namespace gui::analog_eq
