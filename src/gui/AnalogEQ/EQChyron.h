#pragma once

#include "dsp/AnalogEQ/AnalogEQProcessor.h"
#include "gui/Shared/Fonts.h"
#include "gui/Shared/TextSlider.h"
#include "state/PluginState.h"

namespace gui::analog_eq
{
enum class BandID;
class EQChyron : public juce::Component
{
public:
    EQChyron (chowdsp::PluginState& pluginState, dsp::analog_eq::Params& params, const chowdsp::HostContextProvider& hcp);

    void resized() override;
    void paint (juce::Graphics& g) override;
    void setSelectedBand (BandID eqBand);

private:
    BandID band;

    TextSlider lowFreqBoostSlider;
    TextSlider lowFreqCutSlider;
    TextSlider lowFreqCutoffSlider;

    TextSlider highFreqBoostSlider;
    TextSlider highFreqBoostFreqSlider;
    TextSlider highFreqBoostQSlider;

    TextSlider highFreqCutSlider;
    TextSlider highFreqCutFreqSlider;

    chowdsp::ScopedCallbackList callbacks;

    SharedFonts fonts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQChyron)
};
} // namespace gui::analog_eq
