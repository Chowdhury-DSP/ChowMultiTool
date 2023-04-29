#include "SVFEditor.h"
#include "BottomBar.h"
#include "gui/Shared/Colours.h"
#include "gui/Shared/LookAndFeels.h"

namespace gui::svf
{
struct KeytrackButton : IconButton
{
    using IconButton::IconButton;
    using Mode = dsp::svf::KeytrackMonoMode;

    chowdsp::SharedLNFAllocator lnfAllocator;
    chowdsp::EnumChoiceParameter<Mode>* monoModeParam;
    std::optional<chowdsp::ParameterAttachment<chowdsp::EnumChoiceParameter<Mode>>> monoModeAttach;
    juce::UndoManager* um = nullptr;

    void mouseDown (const juce::MouseEvent& e) override
    {
        if (e.mods.isPopupMenu())
        {
            juce::PopupMenu menu;

            juce::PopupMenu::Item lowestNotePriorityItem;
            lowestNotePriorityItem.itemID = 1;
            lowestNotePriorityItem.text = "Lowest Note Priority";
            lowestNotePriorityItem.colour = monoModeParam->get() == Mode::Lowest_Note_Priority ? colours::plotColour : colours::linesColour;
            lowestNotePriorityItem.action = [this]
            {
                monoModeAttach->setValueAsCompleteGesture (
                    (int) *magic_enum::enum_index (Mode::Lowest_Note_Priority), um);
            };
            menu.addItem (std::move (lowestNotePriorityItem));

            juce::PopupMenu::Item highestNotePriorityItem;
            highestNotePriorityItem.itemID = 2;
            highestNotePriorityItem.text = "Highest Note Priority";
            highestNotePriorityItem.colour = monoModeParam->get() == Mode::Highest_Note_Priority ? colours::plotColour : colours::linesColour;
            highestNotePriorityItem.action = [this]
            {
                monoModeAttach->setValueAsCompleteGesture (
                    (int) *magic_enum::enum_index (Mode::Highest_Note_Priority), um);
            };
            menu.addItem (std::move (highestNotePriorityItem));

            menu.setLookAndFeel (lnfAllocator->getLookAndFeel<lnf::MenuLNF>());
            menu.showMenuAsync (juce::PopupMenu::Options {}.withParentComponent (getParentComponent()));

            return;
        }

        IconButton::mouseDown (e);
    }
};

SVFEditor::SVFEditor (State& pluginState, dsp::svf::Params& svfParams, const chowdsp::HostContextProvider& hcp)
    : plot (pluginState, svfParams, hcp),
      paramControls (pluginState, svfParams),
      keytrackButton (std::make_unique<KeytrackButton> ("Vector/fad-keyboard.svg", colours::plotColour, colours::linesColour)),
      keytrackAttach (svfParams.keytrack, pluginState, *keytrackButton),
      arpLimitButton ("Vector/arrows-up-to-line-solid.svg", colours::plotColour, colours::linesColour),
      arpLimitAttach (svfParams.arpLimitMode, pluginState, arpLimitButton)
{
    addAndMakeVisible (plot);
    addAndMakeVisible (paramControls);

    bottomBar = std::make_unique<BottomBar> (pluginState, svfParams);
    addAndMakeVisible (bottomBar.get());

    addAndMakeVisible (keytrackButton.get());
    auto* keytrackButtonCast = static_cast<KeytrackButton*> (keytrackButton.get()); // NOLINT
    keytrackButtonCast->monoModeParam = svfParams.keytrackMonoMode.get();
    keytrackButtonCast->monoModeAttach.emplace (*svfParams.keytrackMonoMode, pluginState, [](int){});
    keytrackButtonCast->um = pluginState.undoManager;

    addChildComponent (arpLimitButton);
    arpLimitButton.setVisible (svfParams.type->get() == dsp::svf::SVFType::ARP);
    modeChangeCallback = pluginState.addParameterListener (svfParams.type,
                                                           chowdsp::ParameterListenerThread::MessageThread,
                                                           [this, &svfParams]
                                                           {
                                                               arpLimitButton.setVisible (svfParams.type->get() == dsp::svf::SVFType::ARP);
                                                           });

    keytrackButton->setTooltip ("Toggles keytrack mode. Right-click to select different note priorities.");
    arpLimitButton.setTooltip ("Toggles the ARP filter's \"Limit\" mode.");
}

void SVFEditor::paint (juce::Graphics& g)
{
    g.setGradientFill (juce::ColourGradient { colours::backgroundLight,
                                              juce::Point { 0.0f, 0.0f },
                                              colours::backgroundDark,
                                              juce::Point { (float) getWidth() * 0.35f, (float) getHeight() * 0.5f },
                                              false });
    g.fillAll();
}

void SVFEditor::resized()
{
    auto bounds = getLocalBounds();
    bottomBar->setBounds (bounds.removeFromBottom (proportionOfHeight (0.075f)));
    paramControls.setBounds (bounds.removeFromRight (proportionOfWidth (0.15f)));
    plot.setBounds (bounds);

    const auto pad = proportionOfWidth (0.005f);
    const auto buttonDim = proportionOfWidth (0.05f);
    keytrackButton->setBounds (bounds.getWidth() - pad - buttonDim, pad, buttonDim, buttonDim);
    arpLimitButton.setBounds (bounds.getWidth() - 2 * (pad + buttonDim), pad, buttonDim, buttonDim);
}
} // namespace gui::svf
