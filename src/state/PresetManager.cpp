#include "PresetManager.h"
#include "ChowMultiTool.h"

namespace state::presets
{
using namespace chowdsp::presets;

constexpr std::string_view presetParamsTag { "preset_params" };
constexpr std::string_view presetWaveshaperStateTag { "ws_state" };

chowdsp::presets::Preset createPresetFromEmbeddedFile (const std::string& path, const cmrc::embedded_filesystem& fs)
{
    const auto presetFile = fs.open (path);
    return { presetFile.begin(), presetFile.size() };
}

PresetManager::PresetManager (ChowMultiTool& plugin)
    : chowdsp::presets::PresetManager (plugin.getState(), &plugin, ".chowpreset"),
      toolParam (*plugin.getState().params.toolParam)
{
    getPresetTree().treeInserter = &PresetTreeInserters::vendorCategoryInserter;
    saverLoader.savePresetState = [this, &plugin]() -> nlohmann::json
    {
        nlohmann::json state;
        state[presetParamsTag] = saverLoader.savePresetParameters();

        auto waveshaperExtraState = chowdsp::JSONSerializer::createBaseElement();
        pfr::for_each_field (*plugin.getState().nonParams.waveshaperExtraState,
                             [&waveshaperExtraState] (const auto& waveshaperStateField)
                             {
                                 waveshaperStateField.serialize (waveshaperExtraState);
                             });
        state[presetWaveshaperStateTag] = std::move (waveshaperExtraState);
        return state;
    };
    saverLoader.loadPresetState = [this, &plugin] (const nlohmann::json& state)
    {
        if (state.contains (presetParamsTag))
            saverLoader.loadPresetParameters (state[presetParamsTag]);

        if (state.contains (presetWaveshaperStateTag))
        {
            using Serializer = chowdsp::JSONSerializer;
            const auto& waveshaperExtraState = state[presetWaveshaperStateTag];
            juce::StringArray namesThatHaveBeenDeserialized {};
            if (const auto numNamesAndVals = Serializer::getNumChildElements (waveshaperExtraState); numNamesAndVals % 2 == 0)
            {
                for (int i = 0; i < numNamesAndVals; i += 2)
                {
                    juce::String name {};
                    chowdsp::Serialization::deserialize<Serializer> (Serializer::getChildElement (waveshaperExtraState, i), name);
                    const auto valueDeserial = Serializer::getChildElement (waveshaperExtraState, i + 1);
                    pfr::for_each_field (*plugin.getState().nonParams.waveshaperExtraState,
                                         [&name, &valueDeserial, &namesThatHaveBeenDeserialized] (auto& waveshaperStateField)
                                         {
                                             if (name == chowdsp::toString (waveshaperStateField.name))
                                             {
                                                 waveshaperStateField.deserialize (valueDeserial);
                                                 namesThatHaveBeenDeserialized.add (name);
                                             }
                                         });
                }
            }
            else
            {
                jassertfalse; // state loading error
            }

            // set all un-matched objects to their default values
            pfr::for_each_field (*plugin.getState().nonParams.waveshaperExtraState,
                                 [&namesThatHaveBeenDeserialized] (auto& waveshaperStateField)
                                 {
                                     if (! namesThatHaveBeenDeserialized.contains (chowdsp::toString (waveshaperStateField.name)))
                                         waveshaperStateField.reset();
                                 });
        }
    };

    const auto fs = cmrc::presets::get_filesystem();
    std::vector<Preset> factoryPresets;
    for (auto&& entry : fs.iterate_directory (""))
    {
        jassert (entry.is_file());
        jassert (fs.exists (entry.filename()));
        factoryPresets.emplace_back (createPresetFromEmbeddedFile (entry.filename(), fs));
    }
    addPresets (std::move (factoryPresets));
    setDefaultPreset (createPresetFromEmbeddedFile ("Init.chowpreset", fs));

    presetsSettings.emplace (*this,
                             *pluginSettings,
                             juce::File::getSpecialLocation (juce::File::userDocumentsDirectory)
                                 .getChildFile ("Chowdhury DSP/Presets/ChowMultiTool"));

    loadDefaultPreset();
}

Preset PresetManager::getUserPresetForState (const juce::String& presetName, nlohmann::json&& presetState) const
{
    std::cout << presetState << std::endl;
    const auto toolParamIndex = presetState[presetParamsTag][toolParam.paramID.toStdString()].get<int>();
    if (toolParamIndex == 0)
        return chowdsp::presets::PresetManager::getUserPresetForState (presetName, std::move (presetState));

    return { presetName, userPresetsVendor, std::move (presetState), toolParam.getAllValueStrings()[toolParamIndex] };
}
} // namespace state::presets
