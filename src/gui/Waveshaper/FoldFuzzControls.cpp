#include "FoldFuzzControls.h"
#include "gui/Shared/Colours.h"
#include "gui/Shared/Fonts.h"

namespace gui::waveshaper
{
FoldFuzzControls::FoldFuzzControls (State& pluginState, dsp::waveshaper::Params& wsParams, const chowdsp::HostContextProvider& hcp)
    : foldSlider (*wsParams.kParam, hcp),
      fuzzSlider (*wsParams.MParam, hcp),
      foldAttach (*wsParams.kParam, pluginState, foldSlider),
      fuzzAttach (*wsParams.MParam, pluginState, fuzzSlider)
{
    for (auto* slider : { &foldSlider, &fuzzSlider })
    {
        slider->setColour (juce::Slider::thumbColourId, colours::boxColour);
        slider->setColour (juce::Slider::textBoxHighlightColourId, colours::boxColour.withAlpha (0.5f));
        addAndMakeVisible (slider);
    }
}

void FoldFuzzControls::paint (juce::Graphics& g)
{
    g.fillAll (colours::backgroundDark);

    auto labelBounds = getLocalBounds().removeFromTop (proportionOfHeight (0.05f));

    g.setFont (juce::Font { SharedFonts{}->robotoBold }.withHeight (0.85f * (float) labelBounds.getHeight()));
    g.setColour (colours::linesColour);

    g.drawFittedText ("Fold", labelBounds.removeFromLeft (proportionOfWidth (0.5f)), juce::Justification::centred, 1);
    g.drawFittedText ("Fuzz", labelBounds, juce::Justification::centred, 1);
}

void FoldFuzzControls::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop (proportionOfHeight (0.05f));
    const auto halfWidth = proportionOfWidth (0.5f);
    foldSlider.setBounds (bounds.removeFromLeft (halfWidth));
    fuzzSlider.setBounds (bounds);

    const auto textBoxHeight = proportionOfHeight (0.04f);
    foldSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, halfWidth, textBoxHeight);
    fuzzSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, halfWidth, textBoxHeight);
}
} // namespace gui::waveshaper
