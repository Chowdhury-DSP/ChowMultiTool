#pragma once

#include "dsp/PultecEQ/PultecEQProcessor.h"
#include "state/PluginState.h"

namespace gui::pultec
{
class DotSlider;

class PultecPlot : public chowdsp::SpectrumPlotBase
{
public:
    PultecPlot (State& pluginState, dsp::pultec::Params& params);
    ~PultecPlot() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void updatePlot();

    chowdsp::GenericFilterPlotter filterPlotter;
    dsp::pultec::PultecEQProcessor pultecEQ;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PultecPlot)
};
} // namespace gui::pultec
