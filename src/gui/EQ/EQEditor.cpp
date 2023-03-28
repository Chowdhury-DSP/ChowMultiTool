#include "EQEditor.h"
#include "BottomBar.h"
#include "gui/Shared/Colours.h"

namespace gui::eq
{
EQEditor::LinearPhaseButton::LinearPhaseButton()
    : juce::Button ("Free-Draw")
{
    const auto fs = cmrc::gui::get_filesystem();
    const auto pencilSVG = fs.open ("Vector/arrow-right-arrow-left-solid.svg");
    const auto pencilIcon = juce::Drawable::createFromImageData (pencilSVG.begin(), pencilSVG.size());
    lpIconOn = pencilIcon->createCopy();
    lpIconOn->replaceColour (juce::Colours::black, colours::thumbColours[0]);
    lpIconOff = pencilIcon->createCopy();
    lpIconOff->replaceColour (juce::Colours::black, colours::linesColour);

    setClickingTogglesState (true);
    setTooltip ("Linear Phase");
}

void EQEditor::LinearPhaseButton::paintButton (juce::Graphics& g, bool, bool)
{
    g.setColour (juce::Colours::black.withAlpha (0.75f));
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 0.1f * (float) getHeight());

    const auto pad = proportionOfWidth (0.2f);
    const auto& pencilIcon = getToggleState() ? lpIconOn : lpIconOff;
    pencilIcon->drawWithin (g, getLocalBounds().reduced (pad).toFloat(), juce::RectanglePlacement::stretchToFit, 1.0f);
}

EQEditor::EQEditor (State& pluginState, dsp::eq::EQToolParams& eqParams)
    : plot (pluginState, eqParams.eqParams),
      paramsView (pluginState, eqParams),
      linearPhaseAttach (eqParams.linearPhaseMode, pluginState, linearPhaseButton)
{
    bottomBar = std::make_unique<BottomBar> (pluginState, eqParams);

    addAndMakeVisible (plot);
    addAndMakeVisible (bottomBar.get());
    addAndMakeVisible (linearPhaseButton);
}

void EQEditor::paint (juce::Graphics& g)
{
    g.setGradientFill (juce::ColourGradient { colours::backgroundLight,
                                              juce::Point { 0.0f, 0.0f },
                                              colours::backgroundDark,
                                              juce::Point { (float) getWidth() * 0.35f, (float) getHeight() * 0.5f },
                                              false });
    g.fillAll();
}

void EQEditor::resized()
{
    auto bounds = getLocalBounds();

    plot.setBounds (bounds);
    bottomBar->setBounds (bounds.removeFromBottom (proportionOfHeight (0.075f)));

    const auto pad = proportionOfWidth (0.005f);
    const auto buttonDim = proportionOfWidth (0.035f);
    linearPhaseButton.setBounds (bounds.getWidth() - pad - buttonDim, pad, buttonDim, buttonDim);
}
} // namespace gui::eq
