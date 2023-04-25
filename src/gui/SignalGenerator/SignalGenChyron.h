#pragma once

#include "dsp/SignalGenerator/SignalGeneratorProcessor.h"
#include "gui/Shared/Fonts.h"
#include "gui/Shared/TextSlider.h"

namespace gui::signal_gen
{
class SignalGenChyron : public juce::Component
{
public:
    SignalGenChyron (chowdsp::PluginState& pluginState, dsp::signal_gen::Params& params, const chowdsp::HostContextProvider& hcp);

    void resized() override;
    void paint (juce::Graphics& g) override;

private:
    TextSlider freqSlider;
    TextSlider gainSlider;

    SharedFonts fonts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SignalGenChyron)
};
} // namespace gui::signal_gen
