#pragma once

#include "dsp/BandSplitter/BandSplitterProcessor.h"
#include "state/PluginState.h"

namespace gui::band_splitter
{
class SlopePicker : public juce::Component
{
public:
    explicit SlopePicker (State& pluginState);

    void paint (juce::Graphics& g) override;

private:
    void mouseDown (const juce::MouseEvent& e) override;
    juce::Rectangle<float> getParamIndexBounds (dsp::band_splitter::Slope) const noexcept;

    State& state;
    dsp::band_splitter::Params& params;
    chowdsp::ParameterAttachment<chowdsp::ChoiceParameter> slopeAttach { *params.slope, state, [this] (int) { repaint(); }};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SlopePicker)
};
} // namespace gui::band_splitter
