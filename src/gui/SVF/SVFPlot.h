#pragma once

#include "dsp/SVF/SVFProcessor.h"
#include "gui/Shared/DotSlider.h"
#include "state/PluginState.h"
#include "SVFChyron.h"

namespace gui::svf
{
class SVFPlot : public chowdsp::SpectrumPlotBase,
                private juce::Timer
{
public:
    SVFPlot (State& pluginState, dsp::svf::Params& params, const chowdsp::HostContextProvider& hcp);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    void updatePlot();
    void keytrackParamChanged (bool keytrackModeOn);

    chowdsp::GenericFilterPlotter filterPlotter;
    dsp::svf::SVFProcessor processor;

    struct KeytrackDotSlider : SpectrumDotSlider
    {
        using SpectrumDotSlider::SpectrumDotSlider;
        double proportionOfLengthToValue (double proportion) override;
        double valueToProportionOfLength (double value) override;
    };

    SpectrumDotSlider freqSlider;
    KeytrackDotSlider keytrackSlider;

    SVFChyron chyron;

    chowdsp::ScopedCallbackList callbacks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SVFPlot)
};
} // namespace gui::svf
