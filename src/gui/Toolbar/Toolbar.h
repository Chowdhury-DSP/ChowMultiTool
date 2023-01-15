#pragma once

#include "state/PluginState.h"

namespace gui
{
class Toolbar : public juce::Component,
                private juce::ChangeListener
{
public:
    explicit Toolbar (State& pluginState);
    ~Toolbar() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;
    void setupUndoRedoButtons();
    void refreshUndoRedoButtons();

    State& state;

    juce::DrawableButton undoButton { "UNDO", juce::DrawableButton::ImageStretched };
    juce::DrawableButton redoButton { "REDO", juce::DrawableButton::ImageStretched };

    juce::ComboBox toolChoiceBox;
    chowdsp::ComboBoxAttachment toolChoiceAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Toolbar)
};
} // namespace gui
