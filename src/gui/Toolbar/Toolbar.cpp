#include "Toolbar.h"

namespace gui
{
Toolbar::Toolbar (State& pluginState)
    : state (pluginState),
      toolChoiceAttachment (state.params.toolParam,
                            state,
                            toolChoiceBox)
{
    setupUndoRedoButtons();

    addAndMakeVisible (toolChoiceBox);
}

Toolbar::~Toolbar()
{
    state.undoManager->removeChangeListener (this);
}

void Toolbar::setupUndoRedoButtons()
{
    addAndMakeVisible (undoButton);
    undoButton.onClick = [this] {
        state.undoManager->undo();
    };

    addAndMakeVisible (redoButton);
    redoButton.onClick = [this] {
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
    g.fillAll (juce::Colours::darkgrey);
}

void Toolbar::resized()
{
    auto bounds = getLocalBounds();
    undoButton.setBounds (bounds.removeFromLeft (60));
    redoButton.setBounds (bounds.removeFromLeft (60));

    bounds.removeFromLeft (25);

    toolChoiceBox.setBounds (bounds.removeFromLeft (100));
}
}
