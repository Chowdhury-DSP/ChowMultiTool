#include "PresetsComponent.h"
#include "gui/Shared/Colours.h"
#include "gui/Shared/ErrorMessageView.h"

namespace gui::presets
{
PresetsComponent::PresetsComponent (chowdsp::presets::PresetManager& presetMgr,
                                    chowdsp::presets::frontend::FileInterface& fileFace)
    : chowdsp::presets::PresetsComponent (presetMgr, &fileFace)
{
    setColour (juce::Label::backgroundColourId, colours::linesColour.withAlpha (0.5f));

    presetNameDisplay.setFont (juce::Font { fonts->robotoBold }.withHeight (18.0f));
    presetNameEditor.setFont (juce::Font { fonts->robotoBold }.withHeight (18.0f));
}

void PresetsComponent::confirmAndDeletePreset (const chowdsp::presets::Preset& presetToDelete,
                                               std::function<void (const chowdsp::presets::Preset&)>&& presetDeleter)
{
    ErrorMessageView::showYesNoBox ("Preset Deletion Warning!",
                                    "Are you sure you want to delete the following preset? "
                                    "This action cannot be undone!\n"
                                        + presetToDelete.getName(),
                                    this,
                                    [&presetToDelete, deleter = std::move (presetDeleter)] (bool shouldDelete)
                                    {
                                        if (shouldDelete)
                                            deleter (presetToDelete);
                                    });
}

void PresetsComponent::confirmAndOverwritePresetFile (const juce::File& presetFile,
                                                      chowdsp::presets::Preset&& preset,
                                                      std::function<void (const juce::File&, chowdsp::presets::Preset&&)>&& presetSaver)
{
    ErrorMessageView::showYesNoBox ("Preset Overwrite Warning!",
                                    "Saving this preset will overwrite an existing file. Are you sure you want to continue?",
                                    this,
                                    [presetFile, preset = std::move (preset), saver = std::move (presetSaver)] (bool shouldOverwrite) mutable
                                    {
                                        if (shouldOverwrite)
                                            saver (presetFile, std::move (preset));
                                    });
}
} // namespace gui::presets
