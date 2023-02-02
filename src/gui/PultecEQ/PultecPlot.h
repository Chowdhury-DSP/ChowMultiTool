#pragma once

#include "dsp/PultecEQ/PultecEQProcessor.h"
#include "state/PluginState.h"

namespace gui::pultec
{
class PultecPlot : public chowdsp::SpectrumPlotBase
{
public:
    PultecPlot (State& pluginState, dsp::pultec::Params& params);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void updatePlot();

    chowdsp::GenericFilterPlotter filterPlotter;
    dsp::pultec::PultecEQProcessor pultecEQ;

    chowdsp::ScopedCallbackList callbacks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PultecPlot)
};
} // namespace gui::pultec
