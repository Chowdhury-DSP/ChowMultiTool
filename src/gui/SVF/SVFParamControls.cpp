#include "SVFParamControls.h"
#include "gui/Shared/Colours.h"
#include "gui/Shared/Fonts.h"

namespace gui::svf
{
SVFParamControls::SVFParamControls (State& pluginState, dsp::svf::Params& params, const chowdsp::HostContextProvider& hcp)
    : svfParams (params),
      modeSlider (*params.mode, hcp),
      qSlider (*params.qParam, hcp),
      dampingSlider (*params.wernerDamping, hcp),
      driveSlider (*params.wernerDrive, hcp),
      modeAttach (*params.mode, pluginState, modeSlider),
      qAttach (*params.qParam, pluginState, qSlider),
      dampingAttach (*params.wernerDamping, pluginState, dampingSlider),
      driveAttach (*params.wernerDrive, pluginState, driveSlider)
{
    for (auto* slider : { &modeSlider, &qSlider, &dampingSlider, &driveSlider })
    {
        slider->setColour (juce::Slider::thumbColourId, colours::boxColour);
        slider->setColour (juce::Slider::textBoxHighlightColourId, colours::boxColour.withAlpha (0.5f));
        addChildComponent (*slider);
    }

    qSlider.setVisible (true);
    updateVisibilities();

    for (auto* param : { (const chowdsp::ChoiceParameter*) svfParams.type.get(),
                         (const chowdsp::ChoiceParameter*) svfParams.wernerType.get(),
                         (const chowdsp::ChoiceParameter*) svfParams.plainType.get() })
    {
        callbacks += {
            pluginState.addParameterListener (*param,
                                              chowdsp::ParameterListenerThread::MessageThread,
                                              [this]
                                              { updateVisibilities(); })
        };
    }
}

void SVFParamControls::updateVisibilities()
{
    using dsp::svf::SVFType;
    dampingSlider.setVisible (svfParams.type->get() == SVFType::Werner);
    driveSlider.setVisible (svfParams.type->get() == SVFType::Werner);
    modeSlider.setVisible ((svfParams.type->get() == SVFType::Werner && svfParams.wernerType->get() == chowdsp::WernerFilterType::MultiMode)
                           || (svfParams.type->get() == SVFType::Plain && svfParams.plainType->get() == dsp::svf::PlainType::MultiMode));
    repaint();
    resized();
}

void SVFParamControls::paint (juce::Graphics& g)
{
    g.fillAll (colours::backgroundDark);

    g.setColour (colours::linesColour);
    auto labelBounds = getLocalBounds().removeFromTop (proportionOfHeight (0.05f));

    g.setFont (juce::Font { SharedFonts{}->robotoBold }.withHeight (0.85f * (float) labelBounds.getHeight()));
    g.setColour (colours::linesColour);

    if (modeSlider.isVisible() && dampingSlider.isVisible())
    {
        const auto quarterWidth = proportionOfWidth (0.25f);
        g.drawFittedText ("Damp", labelBounds.removeFromLeft (quarterWidth), juce::Justification::centredTop, 1);
        g.drawFittedText ("Drive", labelBounds.removeFromLeft (quarterWidth), juce::Justification::centredTop, 1);
        g.drawFittedText ("Mode", labelBounds.removeFromLeft (quarterWidth), juce::Justification::centredTop, 1);
    }
    else if (dampingSlider.isVisible())
    {
        const auto thirdWidth = proportionOfWidth (1.0f / 3.0f);
        g.drawFittedText ("Damp", labelBounds.removeFromLeft (thirdWidth), juce::Justification::centredTop, 1);
        g.drawFittedText ("Drive", labelBounds.removeFromLeft (thirdWidth), juce::Justification::centredTop, 1);
    }
    else if (modeSlider.isVisible())
    {
        g.drawFittedText ("Mode", labelBounds.removeFromLeft (proportionOfWidth (0.5f)), juce::Justification::centredTop, 1);
    }

    g.drawFittedText ("Q", labelBounds, juce::Justification::centredTop, 1);
}

void SVFParamControls::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop (proportionOfHeight (0.05f));
    const auto textBoxHeight = proportionOfHeight (0.04f);

    if (modeSlider.isVisible() && dampingSlider.isVisible())
    {
        const auto quarterWidth = proportionOfWidth (0.25f);
        dampingSlider.setBounds (bounds.removeFromLeft (quarterWidth));
        driveSlider.setBounds (bounds.removeFromLeft (quarterWidth));
        modeSlider.setBounds (bounds.removeFromLeft (quarterWidth));

        dampingSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, quarterWidth, textBoxHeight);
        driveSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, quarterWidth, textBoxHeight);
        modeSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, quarterWidth, textBoxHeight);
    }
    else if (dampingSlider.isVisible())
    {
        const auto thirdWidth = proportionOfWidth (1.0f / 3.0f);
        dampingSlider.setBounds (bounds.removeFromLeft (thirdWidth));
        driveSlider.setBounds (bounds.removeFromLeft (thirdWidth));

        dampingSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, thirdWidth, textBoxHeight);
        driveSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, thirdWidth, textBoxHeight);
    }
    else if (modeSlider.isVisible())
    {
        const auto halfWidth = proportionOfWidth (0.5f);
        modeSlider.setBounds (bounds.removeFromLeft (halfWidth));
        modeSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, halfWidth, textBoxHeight);
    }

    qSlider.setBounds (bounds);
    qSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, qSlider.getWidth(), textBoxHeight);
}
} // namespace gui::svf
