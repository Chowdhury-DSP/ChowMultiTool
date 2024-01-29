#pragma once

#include "dsp/BandSplitter/BandSplitterProcessor.h"
#include "gui/Shared/TextSlider.h"
#include "gui/Shared/Fonts.h"
#include "gui/Shared/Colours.h"

namespace gui::band_splitter
{
class BandSplitterChyron : public juce::Component
{
public:
    BandSplitterChyron (chowdsp::PluginState& pluginState,
                        dsp::band_splitter::Params& bandSplitterParameters,
                        const chowdsp::HostContextProvider& hcp);
    void updateValues();
    void resized() override;
    void paint (juce::Graphics& g) override;

private:
    chowdsp::PluginState& state;
    dsp::band_splitter::Params& bandSplitterParams;

    std::optional<TextSlider> cutoffSliderLow;
    std::optional<TextSlider> cutoffSliderMid;
    std::optional<TextSlider> cutoffSliderHigh;

    chowdsp::ScopedCallbackList callbacks;
    const chowdsp::HostContextProvider& hostContextProvider;

    SharedFonts fonts;
};
} // namespace gui::band_splitter