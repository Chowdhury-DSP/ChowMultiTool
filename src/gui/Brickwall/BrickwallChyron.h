#pragma once

#include "dsp/Brickwall/BrickwallProcessor.h"
#include "gui/Shared/Fonts.h"
#include "gui/Shared/TextSlider.h"

namespace gui::brickwall
{
class BrickwallChyron : public juce::Component
{
public:
    BrickwallChyron (chowdsp::PluginState& pluginState,
                     dsp::brickwall::Params& params,
                     const chowdsp::HostContextProvider& hcp);

    void resized() override;
    void paint (juce::Graphics& g) override;

private:
    chowdsp::PluginState& state;

    TextSlider cutoffSlider;

    SharedFonts fonts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BrickwallChyron)
};
} // namespace gui::brickwall
