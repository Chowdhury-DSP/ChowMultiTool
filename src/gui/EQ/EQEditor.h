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
    EQEditor (State& pluginState, dsp::eq::EQToolParams& eqParams, const chowdsp::HostContextProvider& hcp, EQHelpers& eqHelper);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    EQPlot plot;
    std::unique_ptr<juce::Component> bottomBar;
    chowdsp::ParametersView paramsView;

    IconButton linearPhaseButton;
    chowdsp::ButtonAttachment linearPhaseAttach;
    std::unique_ptr<SpectrumAnalyser> spectrumAnalyser;

    IconButton drawButton;
    IconButton drawCheckButton;
    IconButton drawXButton;

    juce::TooltipWindow tooltips { this, 300 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQEditor)
};
} // namespace gui::eq
