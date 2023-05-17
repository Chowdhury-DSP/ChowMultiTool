#pragma once

#include <pch.h>

class ChowMultiTool;

namespace state::presets
{
chowdsp::presets::Preset createPresetFromEmbeddedFile (const std::string& path, const cmrc::embedded_filesystem& fs);

class PresetManager : public chowdsp::presets::PresetManager
{
public:
    explicit PresetManager (ChowMultiTool& plugin);

    [[nodiscard]] chowdsp::presets::Preset getUserPresetForState (const juce::String& presetName, nlohmann::json&& presetState) const override;
    auto* getPresetSettings() { return &(*presetsSettings); }

    static std::vector<chowdsp::presets::Preset> getFactoryPresets();

private:
    chowdsp::SharedPluginSettings pluginSettings;
    std::optional<chowdsp::presets::frontend::SettingsInterface> presetsSettings;

    const chowdsp::ChoiceParameter& toolParam;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetManager)
};
} // namespace state::presets
