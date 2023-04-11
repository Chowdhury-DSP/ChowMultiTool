#include "BottomBar.h"
#include "dsp/SVF/SVFProcessor.h"
#include "gui/Shared/Colours.h"

namespace gui::svf
{
BottomBar::BottomBar (State& pluginState, dsp::svf::Params& svfParams)
    : filterTypeAttach (svfParams.type, pluginState, filterTypeMenu),
      plainSubTypeAttach (svfParams.plainType, pluginState, plainFilterSubTypeMenu),
      arpSubTypeAttach (svfParams.arpType, pluginState, arpFilterSubTypeMenu),
      wernerSubTypeAttach (svfParams.wernerType, pluginState, wernerFilterSubTypeMenu)
{
    addAndMakeVisible (filterTypeMenu);
    addChildComponent (plainFilterSubTypeMenu);
    addChildComponent (arpFilterSubTypeMenu);
    addChildComponent (wernerFilterSubTypeMenu);

    updateVisibilities (svfParams.type->get());

    typeChangeCallback = pluginState.addParameterListener (svfParams.type,
                                                           chowdsp::ParameterListenerThread::MessageThread,
                                                           [this, &filterType = svfParams.type]
                                                           {
                                                               updateVisibilities (filterType->get());
                                                           });
}

void BottomBar::updateVisibilities (dsp::svf::SVFType svfType)
{
    using dsp::svf::SVFType;
    plainFilterSubTypeMenu.setVisible (svfType == SVFType::Plain);
    arpFilterSubTypeMenu.setVisible (svfType == SVFType::ARP);
    wernerFilterSubTypeMenu.setVisible (svfType == SVFType::Werner);
}

void BottomBar::resized()
{
    auto bounds = getLocalBounds();
    filterTypeMenu.setBounds (bounds.removeFromLeft (proportionOfWidth (0.5f)));
    plainFilterSubTypeMenu.setBounds (bounds);
    arpFilterSubTypeMenu.setBounds (bounds);
    wernerFilterSubTypeMenu.setBounds (bounds);
}

void BottomBar::paint (juce::Graphics& g)
{
    const auto verticalGrad = [this] (juce::Colour top, juce::Colour bottom)
    {
        return juce::ColourGradient::vertical (top, 0.0f, bottom, (float) getHeight());
    };

    auto backgroundGrad = verticalGrad (colours::boxColour.withAlpha (0.0f), colours::boxColour);
    g.setGradientFill (std::move (backgroundGrad));
    g.fillAll();

    g.setGradientFill (verticalGrad (colours::linesColour.withAlpha (0.75f), colours::linesColour));
    const auto halfWidthPos = (float) getWidth() * 0.5f;
    g.drawLine (juce::Line { juce::Point { halfWidthPos, 0.0f }, juce::Point { halfWidthPos, (float) getHeight() } }, 1.0f);
}
} // namespace gui::svf
