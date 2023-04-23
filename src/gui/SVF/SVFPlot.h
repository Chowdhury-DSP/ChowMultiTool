#pragma once

#include "dsp/SVF/SVFProcessor.h"
#include "gui/Shared/DotSlider.h"
#include "state/PluginState.h"

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

    chowdsp::GenericFilterPlotter filterPlotter;
    dsp::svf::SVFProcessor processor;

    SpectrumDotSlider freqSlider;

    chowdsp::ScopedCallbackList callbacks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SVFPlot)
};
} // namespace gui::svf
