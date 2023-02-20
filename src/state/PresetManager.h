#pragma once

#include <pch.h>

class ChowMultiTool;

namespace state::presets
{
class PresetManager : public chowdsp::presets::PresetManager
{
public:
    explicit PresetManager (ChowMultiTool& plugin);

    [[nodiscard]] chowdsp::presets::Preset getUserPresetForState (const juce::String& presetName, nlohmann::json&& presetState) const override;
    auto* getPresetSettings() { return &(*presetsSettings); }

private:
    chowdsp::SharedPluginSettings pluginSettings;
    std::optional<chowdsp::presets::frontend::SettingsInterface> presetsSettings;

    const chowdsp::ChoiceParameter& toolParam;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetManager)
};
}
