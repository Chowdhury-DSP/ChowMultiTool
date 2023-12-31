#include "Toolbar.h"
#include "ChowMultiTool.h"
#include "gui/Shared/Colours.h"
#include "gui/Shared/LookAndFeels.h"
#include "state/presets/PresetManager.h"

namespace gui
{
Toolbar::Toolbar (ChowMultiTool& plugin, chowdsp::OpenGLHelper& oglHelper)
    : state (plugin.getState()),
      toolChoiceAttachment (state.params.toolParam,
                            state,
                            toolChoiceBox),
      presetsFileInterface (plugin.getPresetManager(),
                            static_cast<state::presets::PresetManager&> (plugin.getPresetManager()).getPresetSettings()), //NOLINT
      presetsComp (plugin.getPresetManager(), presetsFileInterface),
      settingsButton (plugin, oglHelper)
{
    setupUndoRedoButtons();

    toolChoiceBox.setColour (juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    toolChoiceBox.setColour (juce::ComboBox::backgroundColourId, colours::linesColour);
    toolChoiceBox.setColour (juce::ComboBox::textColourId, colours::backgroundLight);
    toolChoiceBox.setColour (juce::ComboBox::arrowColourId, colours::backgroundLight);
    toolChoiceBox.setLookAndFeel (lnfAllocator->getLookAndFeel<gui::lnf::MenuLNF>());
    addAndMakeVisible (toolChoiceBox);

    presetsComp.setLookAndFeel (lnfAllocator->getLookAndFeel<gui::lnf::MenuLNF>());
    addAndMakeVisible (presetsComp);

    bypassButton.setColour (juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentWhite);
    bypassButton.setColour (juce::TextButton::ColourIds::buttonColourId, colours::linesColour);
    bypassButton.setColour (juce::TextButton::ColourIds::buttonOnColourId, logo::colours::accentRed);
    bypassButton.setColour (juce::TextButton::ColourIds::textColourOffId, colours::backgroundLight);
    bypassButton.setColour (juce::TextButton::ColourIds::textColourOnId, colours::linesColour);
    bypassButton.setClickingTogglesState (true);
    bypassButton.setLookAndFeel (lnfAllocator->getLookAndFeel<lnf::MenuLNF>());
    addAndMakeVisible (bypassButton);

    addAndMakeVisible (settingsButton);
}

Toolbar::~Toolbar()
{
    state.undoManager->removeChangeListener (this);
}

void Toolbar::setupUndoRedoButtons()
{
    const auto fs = cmrc::gui::get_filesystem();
    const auto undoSVG = fs.open ("Vector/undo-solid.svg");
    auto undoDrawableEnabled = juce::Drawable::createFromImageData (undoSVG.begin(), undoSVG.size());
    undoDrawableEnabled->replaceColour (juce::Colours::black, colours::linesColour);
    auto undoDrawableDisabled = juce::Drawable::createFromImageData (undoSVG.begin(), undoSVG.size());
    undoDrawableDisabled->replaceColour (juce::Colours::black, colours::backgroundLight);
    undoButton.setImages (undoDrawableEnabled.get(),
                          nullptr,
                          nullptr,
                          undoDrawableDisabled.get());
    addAndMakeVisible (undoButton);
    undoButton.onClick = [this]
    {
        state.undoManager->undo();
    };

    const auto redoSVG = fs.open ("Vector/redo-solid.svg");
    auto redoDrawableEnabled = juce::Drawable::createFromImageData (redoSVG.begin(), redoSVG.size());
    redoDrawableEnabled->replaceColour (juce::Colours::black, colours::linesColour);
    auto redoDrawableDisabled = juce::Drawable::createFromImageData (redoSVG.begin(), redoSVG.size());
    redoDrawableDisabled->replaceColour (juce::Colours::black, colours::backgroundLight);
    redoButton.setImages (redoDrawableEnabled.get(),
                          nullptr,
                          nullptr,
                          redoDrawableDisabled.get());
    addAndMakeVisible (redoButton);
    redoButton.onClick = [this]
    {
        state.undoManager->redo();
    };

    state.undoManager->addChangeListener (this);
    refreshUndoRedoButtons();
}

void Toolbar::refreshUndoRedoButtons()
{
    undoButton.setEnabled (state.undoManager->canUndo());
    redoButton.setEnabled (state.undoManager->canRedo());
}

void Toolbar::changeListenerCallback (juce::ChangeBroadcaster* source)
{
    if (source != state.undoManager)
        return;

    refreshUndoRedoButtons();
}

void Toolbar::paint (juce::Graphics& g)
{
    g.fillAll (colours::toolbarGrey);
}

void Toolbar::resized()
{
    auto bounds = getLocalBounds();

    const auto buttonDim = proportionOfHeight (0.7f);
    const auto buttonY = (getHeight() - buttonDim) / 2;

    bounds.reduce (proportionOfWidth (0.01f), 0);
    undoButton.setBounds (juce::Rectangle { buttonDim, buttonDim }.withY (buttonY).withX (bounds.getX()));

    bounds.removeFromLeft (buttonDim + proportionOfWidth (0.0125f));
    redoButton.setBounds (juce::Rectangle { buttonDim, buttonDim }.withY (buttonY).withX (bounds.getX()));

    toolChoiceBox.setBounds (proportionOfWidth (0.1875f),
                             proportionOfHeight (10.0f / 75.0f),
                             proportionOfWidth (0.28125f),
                             proportionOfHeight (55.0f / 75.0f));

    presetsComp.setBounds (proportionOfWidth (0.53125f),
                           proportionOfHeight (10.0f / 75.0f),
                           proportionOfWidth (0.3125f),
                           proportionOfHeight (55.0f / 75.0f));

    bypassButton.setBounds (juce::Rectangle { 2 * buttonDim, buttonDim }
                                .withY (buttonY)
                                .withRight (bounds.getRight() - buttonDim)
                                .withLeft (presetsComp.getRight())
                                .reduced (proportionOfWidth (0.01f), 0));

    settingsButton.setBounds (juce::Rectangle { buttonDim, buttonDim }.withY (buttonY).withRightX (bounds.getRight()));
}
} // namespace gui
