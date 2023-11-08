#pragma once

#include "dsp/EQ/EQProcessor.h"
#include "gui/Shared/Fonts.h"
#include "gui/Shared/TextSlider.h"
#include "state/PluginState.h"

namespace gui::eq
{
constexpr auto numBands = dsp::eq::EQToolParams::numBands;
class EQChyron : public juce::Component
{
public:
    EQChyron (chowdsp::PluginState& pluginState,
              chowdsp::EQ::StandardEQParameters<numBands>& eqParameters,
              const chowdsp::HostContextProvider& hcp);

    void resized() override;
    void paint (juce::Graphics& g) override;
    void setSelectedBand (int newSelectedBand);

private:
    void updateValues();

    std::optional<int> selectedBand { std::nullopt };
    chowdsp::EQ::EQPlotFilterType filterType = chowdsp::EQ::EQPlotFilterType::LPF1;

    chowdsp::PluginState& state;
    chowdsp::EQ::StandardEQParameters<numBands>& eqParams;

    std::optional<TextSlider> freqSlider;
    std::optional<TextSlider> qSlider;
    std::optional<TextSlider> gainSlider;

    chowdsp::ScopedCallbackList callbacks;
    const chowdsp::HostContextProvider& hostContextProvider;

    SharedFonts fonts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQChyron)
};
} // namespace gui::eq
