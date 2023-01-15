#pragma once

#include "dsp/BandSplitter/BandSplitterProcessor.h"
#include "state/PluginState.h"

namespace gui::band_splitter
{
class BandSplitterEditor : public juce::Component
{
public:
    BandSplitterEditor (State& pluginState, dsp::band_splitter::Params& params);

    void resized() override;

private:
    chowdsp::ParametersView paramsView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandSplitterEditor)
};
} // namespace gui::band_splitter
