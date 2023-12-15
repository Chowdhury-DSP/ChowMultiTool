#pragma once

#include "SVFChyron.h"
#include "dsp/SVF/SVFProcessor.h"
#include "gui/Shared/DotSlider.h"
#include "gui/Shared/SpectrumAnalyser.h"
#include "state/PluginState.h"

namespace gui::svf
{
class SVFPlot : public chowdsp::SpectrumPlotBase,
                private juce::Timer
{
public:
    SVFPlot (State& pluginState,
             dsp::svf::Params& params,
             dsp::svf::ExtraState& svfExtraState,
             const chowdsp::HostContextProvider& hcp,
             std::pair<gui::SpectrumAnalyserTask::Optional, gui::SpectrumAnalyserTask::Optional> spectrumAnalyserTasks);
    ~SVFPlot();
    void paint (juce::Graphics& g) override;
    void paintOverChildren (juce::Graphics& g) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent& event) override;

private:
    void timerCallback() override;
    void updatePlot();
    void keytrackParamChanged (bool keytrackModeOn);

    chowdsp::GenericFilterPlotter filterPlotter;
    dsp::svf::ExtraState& extraState;
    dsp::svf::SVFProcessor processor;

    struct KeytrackDotSlider : SpectrumDotSlider
    {
        using SpectrumDotSlider::SpectrumDotSlider;
        double proportionOfLengthToValue (double proportion) override;
        double valueToProportionOfLength (double value) override;
    };

    SpectrumDotSlider freqSlider;
    KeytrackDotSlider keytrackSlider;

    SpectrumAnalyser spectrumAnalyser;
    SVFChyron chyron;

    chowdsp::ScopedCallbackList callbacks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SVFPlot)
};
} // namespace gui::svf
