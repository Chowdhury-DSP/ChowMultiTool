#pragma once

#include "EQChyron.h"
#include "dsp/AnalogEQ/AnalogEQProcessor.h"
#include "gui/Shared/SpectrumAnalyser.h"
#include "state/PluginState.h"

namespace gui::analog_eq
{
class DotSlider;

class AnalogEQPlot : public chowdsp::SpectrumPlotBase
{
public:
    AnalogEQPlot (State& pluginState,
                  dsp::analog_eq::Params& params,
                  dsp::analog_eq::ExtraState& analogEqExtraState,
                  const chowdsp::HostContextProvider& hcp,
                  SpectrumAnalyserTask::PrePostPair spectrumAnalyserTasks);
    ~AnalogEQPlot() override;

    void resized() override;

    enum class BandID
    {
        None,
        Low,
        High_Cut,
        High_Boost,
    };

private:
    void mouseDown (const juce::MouseEvent& event) override;

    void updatePlot();
    void setSelectedBand (BandID bandID);

    chowdsp::GenericFilterPlotter filterPlotter;
    dsp::analog_eq::ExtraState& extraState;
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

    struct PlotPainter : juce::Component
    {
        std::function<void (juce::Graphics&)> painter;
        void paint (juce::Graphics& g) override { painter (g); }
    } plotPainter;

    EQChyron chyron;
    SpectrumAnalyser spectrumAnalyser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalogEQPlot)
};
} // namespace gui::analog_eq
