#include "BrickwallEditor.h"
#include "BottomBar.h"
#include "gui/Shared/Colours.h"
#include "gui/Shared/LookAndFeels.h"

namespace gui::brickwall
{
BrickwallEditor::BrickwallEditor (State& pluginState,
                                  dsp::brickwall::Params& params,
                                  dsp::brickwall::ExtraState& brickwallExtraState,
                                  const chowdsp::HostContextProvider& hcp,
                                  std::pair<optionalSpectrumBackgroundTask, optionalSpectrumBackgroundTask> spectrumAnalyserTasks)
    : extraState (brickwallExtraState),
      plot (pluginState, params, hcp),
      spectrumAnalyser (plot, spectrumAnalyserTasks)
{
    addMouseListener (this, true);
    extraState.isEditorOpen.store (true);
    spectrumAnalyser.setShowPostEQ (extraState.showPostSpectrum.get());
    callbacks += {
        extraState.showPostSpectrum.changeBroadcaster.connect ([this]
                                                               {
                                                                   spectrumAnalyser.setShowPostEQ(extraState.showPostSpectrum.get());
                                                                   spectrumAnalyser.repaint(); }),
    };

    bottomBar = std::make_unique<BottomBar> (pluginState, params);

    addAndMakeVisible (plot);
    addAndMakeVisible (bottomBar.get());
    addAndMakeVisible (spectrumAnalyser);
    spectrumAnalyser.toBehind (bottomBar.get());
    //    spectrumAnalyser.toBehind(&plot.getCutoffSlider());
}

BrickwallEditor::~BrickwallEditor()
{
    removeMouseListener (this);
    extraState.isEditorOpen.store (false);
}

void BrickwallEditor::mouseDown (const juce::MouseEvent& event)
{
    if (event.mods.isPopupMenu())
    {
        chowdsp::SharedLNFAllocator lnfAllocator;
        juce::PopupMenu menu;

        juce::PopupMenu::Item postSpectrumItem;
        postSpectrumItem.itemID = 100;
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

void BrickwallEditor::paint (juce::Graphics& g)
{
    g.setGradientFill (juce::ColourGradient { colours::backgroundLight,
                                              juce::Point { 0.0f, 0.0f },
                                              colours::backgroundDark,
                                              juce::Point { (float) getWidth() * 0.35f, (float) getHeight() * 0.5f },
                                              false });
    g.fillAll();
}

void BrickwallEditor::resized()
{
    auto bounds = getLocalBounds();
    plot.setBounds (bounds);
    bottomBar->setBounds (bounds.removeFromBottom (proportionOfHeight (0.075f)));
    spectrumAnalyser.setBounds (getLocalBounds());
}
} // namespace gui::brickwall
