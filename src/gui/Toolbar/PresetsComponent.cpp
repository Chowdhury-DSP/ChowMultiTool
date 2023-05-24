#include "PresetsComponent.h"
#include "gui/Shared/Colours.h"
#include "gui/Shared/ErrorMessageView.h"

namespace gui::presets
{
PresetsComponent::PresetsComponent (chowdsp::presets::PresetManager& presetMgr, PresetsFileInterface& fileFace)
    : chowdsp::presets::PresetsComponent (presetMgr, &fileFace)
{
    setColour (juce::Label::backgroundColourId, colours::linesColour.withAlpha (0.5f));

    presetNameDisplay.setFont (juce::Font { fonts->robotoBold }.withHeight (18.0f));
    presetNameEditor.setFont (juce::Font { fonts->robotoBold }.withHeight (18.0f));
}

bool PresetsComponent::queryShouldDeletePreset (const chowdsp::presets::Preset& preset)
{
    ErrorMessageView::showYesNoBox ("Preset Deletion Warning!",
                                    "Are you sure you want to delete the following preset? "
                                    "This action cannot be undone!\n"
                                        + preset.getName(),
                                    this,
                                    [this, &preset] (bool shouldDelete)
                                    {
                                        if (shouldDelete)
                                        {
                                            preset.getPresetFile().deleteFile();
                                            if (presetManager.getDefaultPreset() != nullptr)
                                                presetManager.loadPreset (*presetManager.getDefaultPreset());
                                            presetManager.loadUserPresetsFromFolder (presetManager.getUserPresetPath());
                                        }
                                    });
    return true;
}
} // namespace gui::presets
