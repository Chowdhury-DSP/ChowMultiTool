#include "SVFParamControls.h"
#include "gui/Shared/Colours.h"

namespace gui::svf
{
SVFParamControls::SVFParamControls (State& pluginState, dsp::svf::Params& params)
    : svfParams (params),
      modeAttach (*params.mode, pluginState, modeSlider),
      qAttach (*params.qParam, pluginState, qSlider),
      dampingAttach (*params.wernerDamping, pluginState, dampingSlider),
      driveAttach (*params.wernerDrive, pluginState, driveSlider)
{
    for (auto* slider : { &modeSlider, &qSlider, &dampingSlider, &driveSlider })
    {
        slider->setSliderStyle (juce::Slider::LinearVertical);
        slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 15);
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
    auto bounds = getLocalBounds();

    if (modeSlider.isVisible() && dampingSlider.isVisible())
    {
        const auto quarterWidth = proportionOfWidth (0.25f);
        g.drawFittedText ("Damp", bounds.removeFromLeft (quarterWidth), juce::Justification::centredTop, 1);
        g.drawFittedText ("Drive", bounds.removeFromLeft (quarterWidth), juce::Justification::centredTop, 1);
        g.drawFittedText ("Mode", bounds.removeFromLeft (quarterWidth), juce::Justification::centredTop, 1);
    }
    else if (dampingSlider.isVisible())
    {
        const auto thirdWidth = proportionOfWidth (1.0f / 3.0f);
        g.drawFittedText ("Damp", bounds.removeFromLeft (thirdWidth), juce::Justification::centredTop, 1);
        g.drawFittedText ("Drive", bounds.removeFromLeft (thirdWidth), juce::Justification::centredTop, 1);
    }
    else if (modeSlider.isVisible())
    {
        g.drawFittedText ("Mode", bounds.removeFromLeft (proportionOfWidth (0.5f)), juce::Justification::centredTop, 1);
    }

    g.drawFittedText ("Q", bounds, juce::Justification::centredTop, 1);
}

void SVFParamControls::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop (proportionOfHeight (0.05f));

    if (modeSlider.isVisible() && dampingSlider.isVisible())
    {
        const auto quarterWidth = proportionOfWidth (0.25f);
        dampingSlider.setBounds (bounds.removeFromLeft (quarterWidth));
        driveSlider.setBounds (bounds.removeFromLeft (quarterWidth));
        modeSlider.setBounds (bounds.removeFromLeft (quarterWidth));
    }
    else if (dampingSlider.isVisible())
    {
        const auto thirdWidth = proportionOfWidth (1.0f / 3.0f);
        dampingSlider.setBounds (bounds.removeFromLeft (thirdWidth));
        driveSlider.setBounds (bounds.removeFromLeft (thirdWidth));
    }
    else if (modeSlider.isVisible())
    {
        modeSlider.setBounds (bounds.removeFromLeft (proportionOfWidth (0.5f)));
    }

    qSlider.setBounds (bounds);
}
} // namespace gui::svf
