#include "AnalogEQEditor.h"
#include "gui/Shared/Colours.h"

namespace gui::analog_eq
{
AnalogEQEditor::AnalogEQEditor (State& pluginState,
                                dsp::analog_eq::Params& params,
                                dsp::analog_eq::ExtraState& analogEqExtraState,
                                const chowdsp::HostContextProvider& hcp,
                                SpectrumAnalyserTask::PrePostPair spectrumAnalyserTasks)
    : plot (pluginState, params, analogEqExtraState, hcp, spectrumAnalyserTasks),
      extraState(analogEqExtraState)
{
    addAndMakeVisible (plot);
}

void AnalogEQEditor::paint (juce::Graphics& g)
{
    g.setGradientFill (juce::ColourGradient { colours::backgroundLight,
                                              juce::Point { 0.0f, 0.0f },
                                              colours::backgroundDark,
                                              juce::Point { (float) getWidth() * 0.35f, (float) getHeight() * 0.5f },
                                              false });
    g.fillAll();
}

void AnalogEQEditor::resized()
{
    plot.setBounds (getLocalBounds());
}
} // namespace gui::analog_eq
