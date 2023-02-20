#pragma once

#include "state/PluginState.h"

class ChowMultiTool;

namespace gui
{
class Toolbar : public juce::Component,
                private juce::ChangeListener
{
public:
    explicit Toolbar (ChowMultiTool& plugin);
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

    chowdsp::presets::frontend::FileInterface presetsFileInterface;
    chowdsp::presets::PresetsComponent presetsComp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Toolbar)
};
} // namespace gui
