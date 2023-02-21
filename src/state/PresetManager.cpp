#include "PresetManager.h"
#include "ChowMultiTool.h"

namespace state::presets
{
using namespace chowdsp::presets;

PresetManager::PresetManager (ChowMultiTool& plugin)
    : chowdsp::presets::PresetManager (plugin.getState(), &plugin, ".chowpreset"),
      toolParam (*plugin.getState().params.toolParam)
{
    getPresetTree().treeInserter = &PresetTreeInserters::vendorCategoryInserter;

    const auto fs = cmrc::presets::get_filesystem();
    const auto createPresetFromEmbeddedFile = [&fs] (const std::string& path) -> Preset
    {
        const auto presetFile = fs.open (path);
        return { presetFile.begin(), presetFile.size() };
    };

    std::vector<Preset> factoryPresets;
    for (auto&& entry : fs.iterate_directory (""))
    {
        jassert (entry.is_file());
        jassert (fs.exists (entry.filename()));
        factoryPresets.emplace_back (createPresetFromEmbeddedFile (entry.filename()));
    }
    addPresets (std::move (factoryPresets));
    setDefaultPreset (createPresetFromEmbeddedFile ("Init.chowpreset"));

    presetsSettings.emplace (*this,
                             *pluginSettings,
                             juce::File::getSpecialLocation (juce::File::userDocumentsDirectory)
                                 .getChildFile ("Chowdhury DSP/Presets/ChowMultiTool"));

    loadDefaultPreset();
}

Preset PresetManager::getUserPresetForState (const juce::String& presetName, nlohmann::json&& presetState) const
{
    const auto toolParamIndex = presetState[toolParam.paramID.toStdString()].get<int>();
    if (toolParamIndex == 0)
        return chowdsp::presets::PresetManager::getUserPresetForState (presetName, std::move (presetState));

    return { presetName, userPresetsVendor, std::move (presetState), toolParam.getAllValueStrings()[toolParamIndex] };
}
} // namespace state::presets
