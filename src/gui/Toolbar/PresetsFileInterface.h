#pragma once

#include <pch.h>

namespace gui::presets
{
class PresetsFileInterface : public chowdsp::presets::frontend::FileInterface
{
public:
    PresetsFileInterface (chowdsp::presets::PresetManager& manager,
                          chowdsp::presets::frontend::SettingsInterface* settingsInterface);

    void deleteCurrentPreset() override;

private:
    chowdsp::presets::PresetManager& presetManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetsFileInterface)
};
}
