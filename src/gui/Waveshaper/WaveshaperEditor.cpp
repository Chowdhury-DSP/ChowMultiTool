#include "WaveshaperEditor.h"
#include "BottomBar.h"
#include "gui/Shared/Colours.h"

namespace gui::waveshaper
{
WaveshaperEditor::FreeDrawButton::FreeDrawButton()
    : juce::Button ("Free-Draw")
{
    const auto fs = cmrc::gui::get_filesystem();
    const auto pencilSVG = fs.open ("Vector/pencil-solid.svg");
    const auto pencilIcon = juce::Drawable::createFromImageData (pencilSVG.begin(), pencilSVG.size());
    pencilIconOn = pencilIcon->createCopy();
    pencilIconOn->replaceColour (juce::Colours::black, colours::plotColour);
    pencilIconOff = pencilIcon->createCopy();
    pencilIconOff->replaceColour (juce::Colours::black, colours::linesColour);

    setClickingTogglesState (true);
}

void WaveshaperEditor::FreeDrawButton::paintButton (juce::Graphics& g, bool, bool)
{
    g.setColour (juce::Colours::black.withAlpha (0.75f));
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 0.1f * (float) getHeight());

    const auto pad = proportionOfWidth (0.2f);
    const auto& pencilIcon = getToggleState() ? pencilIconOn : pencilIconOff;
    pencilIcon->drawWithin (g, getLocalBounds().reduced (pad).toFloat(), juce::RectanglePlacement::stretchToFit, 1.0f);
}

WaveshaperEditor::WaveshaperEditor (State& pluginState, dsp::waveshaper::Params& wsParams)
    : params (wsParams),
      plot (pluginState, wsParams),
      foldFuzzControls (pluginState, wsParams)
{
    bottomBar = std::make_unique<BottomBar> (pluginState, wsParams);

    addAndMakeVisible (plot);
    addAndMakeVisible (bottomBar.get());

    addChildComponent (foldFuzzControls);
    foldFuzzControls.setVisible (params.shapeParam->get() == dsp::waveshaper::Shapes::Fold_Fuzz);

    addChildComponent (freeDrawButton);
    freeDrawButton.setVisible (wsParams.shapeParam->get() == dsp::waveshaper::Shapes::Free_Draw);
    freeDrawButton.onStateChange = [this]
    { plot.toggleDrawMode (freeDrawButton.getToggleState()); };

    callbacks += {
        pluginState.addParameterListener (
            *wsParams.shapeParam,
            chowdsp::ParameterListenerThread::MessageThread,
            [this]
            {
                foldFuzzControls.setVisible (params.shapeParam->get() == dsp::waveshaper::Shapes::Fold_Fuzz);
                resized();
            }),
        pluginState.addParameterListener (
            *wsParams.shapeParam,
            chowdsp::ParameterListenerThread::MessageThread,
            [this, &wsParams]
            {
                const auto isFreeDrawMode = wsParams.shapeParam->get() == dsp::waveshaper::Shapes::Free_Draw;
                freeDrawButton.setVisible (isFreeDrawMode);
                if (! isFreeDrawMode)
                    freeDrawButton.setToggleState (false, juce::sendNotification);
            }),
    };
}

void WaveshaperEditor::resized()
{
    auto bounds = getLocalBounds();
    bottomBar->setBounds (bounds.removeFromBottom (proportionOfHeight (0.075f)));

    if (params.shapeParam->get() == dsp::waveshaper::Shapes::Fold_Fuzz)
        foldFuzzControls.setBounds (bounds.removeFromRight (proportionOfWidth (0.15f)));

    plot.setBounds (bounds);

    const auto pad = proportionOfWidth (0.005f);
    const auto dim = proportionOfWidth (0.05f);
    freeDrawButton.setBounds (bounds.getWidth() - pad - dim, pad, dim, dim);
}

void WaveshaperEditor::paint (juce::Graphics& g)
{
    g.setGradientFill (juce::ColourGradient { colours::backgroundLight,
                                              juce::Point { 0.0f, 0.0f },
                                              colours::backgroundDark,
                                              juce::Point { (float) getWidth() * 0.35f, (float) getHeight() * 0.5f },
                                              false });
    g.fillAll();
}
} // namespace gui::waveshaper
