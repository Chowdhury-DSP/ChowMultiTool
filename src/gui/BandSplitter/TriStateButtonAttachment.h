#pragma once

#include "state/PluginState.h"

namespace dsp::band_splitter
{
enum class BandState;
}

namespace gui::band_splitter
{
using BandState = dsp::band_splitter::BandState;
class TriStateButtonAttachment : public juce::Button::Listener
{
public:
    TriStateButtonAttachment (State& pluginState,
                              juce::Button& triStateButton,
                              std::pair<BandState, int>& currentState);

    ~TriStateButtonAttachment() override;

    void updateButtonState();

private:
    void buttonClicked (juce::Button* button) override;

    void setParametersFromUI (BandState newBandState);

    chowdsp::BoolParameter& threeBandParam;
    chowdsp::BoolParameter& fourBandParam;
    chowdsp::ParameterAttachment<chowdsp::BoolParameter> threeBandAttachment;
    chowdsp::ParameterAttachment<chowdsp::BoolParameter> fourBandAttachment;

    juce::Button* bandStateButton = nullptr;
    juce::UndoManager* um = nullptr;
    std::pair<BandState, int>& currentState;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TriStateButtonAttachment)
};
} // namespace gui::band_splitter