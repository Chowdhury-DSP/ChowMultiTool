#if HAS_CLAP_JUCE_EXTENSIONS

#include "PresetDiscovery.h"
#include "PresetManager.h"

namespace state::presets::discovery
{
static constexpr clap_plugin_id plugin_id {
    .abi = "clap",
    .id = CHOWMULTITOOL_CLAP_ID,
};

static auto getUserPresetsFolder()
{
    chowdsp::SharedPluginSettings pluginSettings;
    pluginSettings->initialise (settingsFilePath, 0);

    static constexpr auto propertyID = chowdsp::presets::frontend::SettingsInterface::userPresetsDirID;
    if (! pluginSettings->hasProperty (propertyID))
        return juce::File {};

    const auto userPresetsPath = pluginSettings->getProperty<juce::String> (propertyID);
    return juce::File { userPresetsPath };
}

//==============================================================================
struct FactoryPresetsProvider : chowdsp::presets::discovery::EmbeddedPresetsProvider
{
    static constexpr clap_preset_discovery_provider_descriptor descriptor {
        .clap_version = CLAP_VERSION_INIT,
        .id = "org.chowdsp.ChowMultiTool.factory-presets",
        .name = "ChowMultiTool Factory Presets Provider",
        .vendor = "ChowDSP"
    };

    static constexpr clap_preset_discovery_location factoryPresetsLocation {
        .flags = CLAP_PRESET_DISCOVERY_IS_FACTORY_CONTENT,
        .name = "ChowMultiTool Factory Presets Location",
        .kind = CLAP_PRESET_DISCOVERY_LOCATION_PLUGIN,
        .location = nullptr,
    };

    explicit FactoryPresetsProvider (const clap_preset_discovery_indexer* indexer)
        : EmbeddedPresetsProvider (plugin_id, descriptor, factoryPresetsLocation, indexer)
    {
    }

    std::vector<chowdsp::presets::Preset> getPresets() override
    {
        return PresetManager::getFactoryPresets();
    }
};

//==============================================================================
struct UserPresetsProvider : chowdsp::presets::discovery::FilePresetsProvider
{
    static constexpr clap_preset_discovery_provider_descriptor descriptor {
        .clap_version = CLAP_VERSION_INIT,
        .id = "org.chowdsp.ChowMultiTool.user-presets",
        .name = "ChowMultiTool User Presets Provider",
        .vendor = "User"
    };


    static constexpr clap_preset_discovery_filetype filetype {
        .name = "User Preset Filetype",
        .description = "User preset filetype for ChowMultiTool",
        .file_extension = "chowpreset",
    };

    explicit UserPresetsProvider (const clap_preset_discovery_indexer* indexer)
        : FilePresetsProvider (plugin_id, descriptor, filetype, indexer)
    {
    }

    bool fillInLocation(clap_preset_discovery_location& location) override
    {
        const auto userPresetsFolder = getUserPresetsFolder();
        if (userPresetsFolder == juce::File {} || ! userPresetsFolder.isDirectory())
            return false;

        location.name = "ChowMultiTool User Presets Location";
        location.location = userPresetsFolder.getFullPathName().toRawUTF8();
        return true;
    }
};

//==============================================================================
uint32_t count (const clap_preset_discovery_factory*)
{
    const auto userPresetsFolder = getUserPresetsFolder();
    if (userPresetsFolder == juce::File {} || ! userPresetsFolder.isDirectory())
        return 1;
    return 2;
}

const clap_preset_discovery_provider_descriptor_t* get_descriptor (
    const clap_preset_discovery_factory*,
    uint32_t index)
{
    if (index == 0)
        return &FactoryPresetsProvider::descriptor;

    if (index == 1)
        return &UserPresetsProvider::descriptor;

    return nullptr;
}

const clap_preset_discovery_provider_t* create (
    const clap_preset_discovery_factory*,
    const clap_preset_discovery_indexer_t* indexer,
    const char* provider_id)
{
    if (strcmp (provider_id, FactoryPresetsProvider::descriptor.id) == 0)
    {
        auto* provider = new FactoryPresetsProvider { indexer };
        return provider->provider();
    }

    if (strcmp (provider_id, UserPresetsProvider::descriptor.id) == 0)
    {
        auto* provider = new UserPresetsProvider { indexer };
        return provider->provider();
    }

    return nullptr;
}
} // namespace state::presets::discovery
#endif
