#pragma once

#include "gui/Shared/Fonts.h"

namespace gui::presets
{
class PresetsComponent : public chowdsp::presets::PresetsComponent
{
public:
    PresetsComponent (chowdsp::presets::PresetManager& presetManager, chowdsp::presets::frontend::FileInterface& fileInterface);

    void confirmAndDeletePreset (const chowdsp::presets::Preset& presetToDelete,
                                 std::function<void (const chowdsp::presets::Preset&)>&& presetDeleter) override;

    void confirmAndOverwritePresetFile (const juce::File& presetFile,
                                        chowdsp::presets::Preset&& preset,
                                        std::function<void (const juce::File&, chowdsp::presets::Preset&&)>&& presetSaver) override;

private:
    SharedFonts fonts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetsComponent)
};
} // namespace gui::presets
