#pragma once

#include "PresetsComponent.h"
#include "SettingsButton.h"
#include "state/PluginState.h"

class ChowMultiTool;

namespace gui
{
class Toolbar : public juce::Component,
                private juce::ChangeListener
{
public:
    Toolbar (ChowMultiTool& plugin, chowdsp::OpenGLHelper& oglHelper);
    ~Toolbar() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;
    void setupUndoRedoButtons();
    void refreshUndoRedoButtons();

    State& state;
    chowdsp::SharedLNFAllocator lnfAllocator;

    juce::DrawableButton undoButton { "UNDO", juce::DrawableButton::ImageStretched };
    juce::DrawableButton redoButton { "REDO", juce::DrawableButton::ImageStretched };

    juce::ComboBox toolChoiceBox;
    chowdsp::ComboBoxAttachment toolChoiceAttachment;

    chowdsp::presets::frontend::FileInterface presetsFileInterface;
    presets::PresetsComponent presetsComp;

    juce::TextButton bypassButton { "Bypass" };
    chowdsp::ButtonAttachment bypassAttach { state.params.bypassParam, state, bypassButton };

    SettingsButton settingsButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Toolbar)
};
} // namespace gui
