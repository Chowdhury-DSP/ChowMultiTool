#pragma once

#include "dsp/BandSplitter/BandSplitterProcessor.h"
#include "gui/Shared/Colours.h"
#include "gui/Shared/Fonts.h"
#include "gui/Shared/TextSlider.h"

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

    std::optional<TextSlider> cutoffSlider1;
    std::optional<TextSlider> cutoffSlider2;
    std::optional<TextSlider> cutoffSlider3;

    chowdsp::ScopedCallbackList callbacks;
    const chowdsp::HostContextProvider& hostContextProvider;

    SharedFonts fonts;
};
} // namespace gui::band_splitter