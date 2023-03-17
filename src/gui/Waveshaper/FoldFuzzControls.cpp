#include "FoldFuzzControls.h"
#include "gui/Shared/Colours.h"

namespace gui::waveshaper
{
FoldFuzzControls::FoldFuzzControls (State& pluginState, dsp::waveshaper::Params& wsParams)
    : foldAttach (*wsParams.kParam, pluginState, foldSlider),
      fuzzAttach (*wsParams.MParam, pluginState, fuzzSlider)
{
    shapeChangeCallback = pluginState.addParameterListener (
        *wsParams.shapeParam,
        chowdsp::ParameterListenerThread::MessageThread,
        [this]
        {
            if (auto* parent = getParentComponent())
                parent->resized();
        });

    foldSlider.setSliderStyle (juce::Slider::LinearVertical);
    foldSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 15);
    addAndMakeVisible (foldSlider);

    fuzzSlider.setSliderStyle (juce::Slider::LinearVertical);
    fuzzSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 15);
    addAndMakeVisible (fuzzSlider);
}

void FoldFuzzControls::paint (juce::Graphics& g)
{
    g.fillAll (colours::backgroundDark);

    g.setColour (colours::linesColour);
    auto bounds = getLocalBounds();
    g.drawFittedText ("Fold", bounds.removeFromLeft (proportionOfWidth (0.5f)), juce::Justification::centredTop, 1);
    g.drawFittedText ("Fuzz", bounds, juce::Justification::centredTop, 1);
}

void FoldFuzzControls::resized()
{
    auto bounds = getLocalBounds();
    foldSlider.setBounds (bounds.removeFromLeft (proportionOfWidth (0.5f)));
    fuzzSlider.setBounds (bounds);
}
} // namespace gui::waveshaper
