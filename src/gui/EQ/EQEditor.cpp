#include "EQEditor.h"
#include "BottomBar.h"
#include "gui/Shared/Colours.h"
#include "gui/Shared/LookAndFeels.h"

namespace gui::eq
{
EQEditor::EQEditor (State& pluginState,
                    dsp::eq::EQToolParams& eqParams,
                    dsp::eq::ExtraState& eqExtraState,
                    const chowdsp::HostContextProvider& hcp,
                    std::pair<SpectrumAnalyserTask&, SpectrumAnalyserTask&> spectrumAnalyserTasks)
    : params (eqParams),
      extraState (eqExtraState),
      plot (pluginState, eqParams.eqParams, hcp),
      paramsView (pluginState, eqParams),
      linearPhaseButton ("Vector/arrow-right-arrow-left-solid.svg", colours::thumbColours[0], colours::linesColour),
      linearPhaseAttach (eqParams.linearPhaseMode, pluginState, linearPhaseButton),
      spectrumAnalyser (plot, spectrumAnalyserTasks),
      drawButton ("Vector/pencil-solid.svg", colours::linesColour, colours::linesColour),
      drawCheckButton ("Vector/square-check-regular.svg", colours::linesColour, colours::linesColour),
      drawXButton ("Vector/rectangle-xmark-regular.svg", colours::linesColour, colours::linesColour)
{
    addMouseListener (this, true);
    extraState.isEditorOpen.store (true);
    spectrumAnalyser.showPreEQ = extraState.showPreSpectrum.get();
    spectrumAnalyser.showPostEQ = extraState.showPostSpectrum.get();
    callbacks += {
        extraState.showPreSpectrum.changeBroadcaster.connect ([this]
                                                              {
            spectrumAnalyser.showPreEQ = extraState.showPreSpectrum.get();
            spectrumAnalyser.repaint(); }),
        extraState.showPostSpectrum.changeBroadcaster.connect ([this]
                                                               {
            spectrumAnalyser.showPostEQ = extraState.showPostSpectrum.get();
            spectrumAnalyser.repaint(); }),
    };

    bottomBar = std::make_unique<BottomBar> (pluginState, eqParams);

    addAndMakeVisible (plot);
    addAndMakeVisible (bottomBar.get());
    addAndMakeVisible (linearPhaseButton);
    addAndMakeVisible (spectrumAnalyser);

    linearPhaseButton.setTooltip ("Linear Phase");

    addAndMakeVisible (drawButton);
    drawButton.onClick = [this]
    {
        linearPhaseButton.setVisible (false);
        drawButton.setVisible (false);
        drawCheckButton.setVisible (true);
        drawXButton.setVisible (true);
        plot.toggleDrawView (true, false);
    };
    addChildComponent (drawCheckButton);
    drawCheckButton.onClick = [this]
    {
        linearPhaseButton.setVisible (true);
        drawButton.setVisible (true);
        drawCheckButton.setVisible (false);
        drawXButton.setVisible (false);
        plot.toggleDrawView (true, true);
    };
    addChildComponent (drawXButton);
    drawXButton.onClick = [this]
    {
        linearPhaseButton.setVisible (true);
        drawButton.setVisible (true);
        drawCheckButton.setVisible (false);
        drawXButton.setVisible (false);
        plot.toggleDrawView (false, false);
    };
}

EQEditor::~EQEditor()
{
    removeMouseListener (this);
    extraState.isEditorOpen.store (false);
}

void EQEditor::mouseDown (const juce::MouseEvent& event)
{
    if (event.mods.isPopupMenu())
    {
        chowdsp::SharedLNFAllocator lnfAllocator;
        juce::PopupMenu menu;

        juce::PopupMenu::Item preSpectrumItem;
        preSpectrumItem.itemID = 100;
        preSpectrumItem.text = extraState.showPreSpectrum.get() ? "Disable Pre-EQ Visualizer" : "Enable Pre-EQ Visualizer";
        preSpectrumItem.action = [this]
        {
            extraState.showPreSpectrum.set (! extraState.showPreSpectrum.get());
        };
        menu.addItem (preSpectrumItem);

        juce::PopupMenu::Item postSpectrumItem;
        postSpectrumItem.itemID = 101;
        postSpectrumItem.text = extraState.showPostSpectrum.get() ? "Disable Post-EQ Visualizer" : "Enable Post-EQ Visualizer";
        postSpectrumItem.action = [this]
        {
            extraState.showPostSpectrum.set (! extraState.showPostSpectrum.get());
        };
        menu.addItem (postSpectrumItem);

        menu.setLookAndFeel (lnfAllocator->getLookAndFeel<lnf::MenuLNF>());
        menu.showMenuAsync (juce::PopupMenu::Options {}
                                .withParentComponent (getParentComponent()));
    }
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

    drawButton.setBounds (linearPhaseButton.getBoundsInParent().translated (-pad - buttonDim, 0));
    drawCheckButton.setBounds (drawButton.getBoundsInParent());
    drawXButton.setBounds (linearPhaseButton.getBoundsInParent());
    spectrumAnalyser.setBounds (getLocalBounds());
    spectrumAnalyser.toBack();
}
} // namespace gui::eq
