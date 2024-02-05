#pragma once

#include "EQPlot.h"
#include "gui/Shared/IconButton.h"
#include "gui/Shared/SpectrumAnalyser.h"

// @TODO:
// - Figure out type-in values for chyron
// - Better controls for Q values

namespace gui::eq
{
class EQEditor : public juce::Component
{
public:
    EQEditor (State& pluginState,
              dsp::eq::EQToolParams& eqParams,
              dsp::eq::ExtraState& extraState,
              const chowdsp::HostContextProvider& hcp,
              SpectrumAnalyserTask::PrePostPair spectrumAnalyserTasks);
    ~EQEditor() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    void mouseDown (const juce::MouseEvent& event) override;

private:
    dsp::eq::ExtraState& extraState;
    chowdsp::ScopedCallbackList callbacks;

    EQPlot plot;
    std::unique_ptr<juce::Component> bottomBar;
    chowdsp::ParametersView paramsView;

    IconButton linearPhaseButton;
    chowdsp::ButtonAttachment linearPhaseAttach;
    SpectrumAnalyser spectrumAnalyser;

    IconButton drawButton;
    IconButton drawCheckButton;
    IconButton drawXButton;

    juce::TooltipWindow tooltips { this, 300 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQEditor)
};
} // namespace gui::eq
