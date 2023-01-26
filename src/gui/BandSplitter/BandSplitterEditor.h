#pragma once

#include "BandSplitterPlot.h"

namespace gui::band_splitter
{
class BandSplitterEditor : public juce::Component
{
public:
    BandSplitterEditor (State& pluginState, dsp::band_splitter::Params& params);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    juce::ComboBox slopeChoiceBox;
    chowdsp::ComboBoxAttachment slopeChoiceAttachment;

    BandSplitterPlot bandSplitterPlot;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandSplitterEditor)
};
} // namespace gui::band_splitter
