#if HAS_CLAP_JUCE_EXTENSIONS

#include "PresetDiscovery.h"
#include "PresetManager.h"

JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wunused-parameter")
#include <clap/helpers/preset-discovery-provider.hh>
#include <clap/helpers/preset-discovery-provider.hxx>
JUCE_END_IGNORE_WARNINGS_GCC_LIKE

namespace state::presets::discovery
{
constexpr clap_plugin_id plugin_id {
    .abi = "clap",
    .id = "org.chowdsp.ChowMultiTool" // TODO: get this in an automated way...
};

struct FactoryPresetDiscoveryProvider
#if JUCE_DEBUG
    : clap::helpers::PresetDiscoveryProvider<clap::helpers::MisbehaviourHandler::Terminate, clap::helpers::CheckingLevel::Maximal>
#else
    : clap::helpers::PresetDiscoveryProvider<clap::helpers::MisbehaviourHandler::Ignore, clap::helpers::CheckingLevel::Minimal>
#endif
{
    static constexpr clap_preset_discovery_provider_descriptor descriptor {
        .clap_version = CLAP_VERSION,
        .id = "org.chowdsp.ChowMultiTool.factory-presets",
        .name = "ChowMultiTool factory preset provider",
        .vendor = "ChowDSP"
    };

    FactoryPresetDiscoveryProvider (const clap_preset_discovery_provider_descriptor* desc,
                                    const clap_preset_discovery_indexer* indexer)
        : PresetDiscoveryProvider (desc, indexer)
    {
    }

    static constexpr clap_preset_discovery_location factoryPresetsLocation {
        .flags = CLAP_PRESET_DISCOVERY_IS_FACTORY_CONTENT,
        .name = "Factory Presets Location",
        .kind = CLAP_PRESET_DISCOVERY_LOCATION_PLUGIN,
        .location = nullptr,
    };

    bool init() noexcept override
    {
        indexer()->declare_location (indexer(), &factoryPresetsLocation);
        return true;
    }

    bool getMetadata (uint32_t location_kind,
                      const char* location,
                      const clap_preset_discovery_metadata_receiver_t* metadata_receiver) noexcept override
    {
        if (location_kind != CLAP_PRESET_DISCOVERY_LOCATION_PLUGIN || location != nullptr)
            return false;

        using chowdsp::presets::Preset;
        const auto fs = cmrc::presets::get_filesystem();
        for (auto&& entry : fs.iterate_directory (""))
        {
            jassert (entry.is_file());
            jassert (fs.exists (entry.filename()));
            const auto factoryPreset = createPresetFromEmbeddedFile (entry.filename(), fs);

            std::cout << "Indexing factory preset: " << factoryPreset.getName() << std::endl;
            if (metadata_receiver->begin_preset (metadata_receiver, factoryPreset.getName().toRawUTF8(), factoryPreset.getName().toRawUTF8()))
            {
                metadata_receiver->add_plugin_id (metadata_receiver, &plugin_id);
                metadata_receiver->add_creator (metadata_receiver, factoryPreset.getVendor().toRawUTF8());

                if (factoryPreset.getCategory().isNotEmpty())
                    metadata_receiver->add_feature (metadata_receiver, factoryPreset.getCategory().toRawUTF8());
            }
            else
            {
                break;
            }
        }

        return true;
    }
};

struct UserPresetDiscoveryProvider
#if JUCE_DEBUG
    : clap::helpers::PresetDiscoveryProvider<clap::helpers::MisbehaviourHandler::Terminate, clap::helpers::CheckingLevel::Maximal>
#else
    : clap::helpers::PresetDiscoveryProvider<clap::helpers::MisbehaviourHandler::Ignore, clap::helpers::CheckingLevel::Minimal>
#endif
{
    static constexpr clap_preset_discovery_provider_descriptor descriptor {
        .clap_version = CLAP_VERSION,
        .id = "org.chowdsp.ChowMultiTool.user-presets",
        .name = "ChowMultiTool user preset provider",
        .vendor = "User"
    };

    UserPresetDiscoveryProvider (const clap_preset_discovery_provider_descriptor* desc,
                                 const clap_preset_discovery_indexer* indexer)
        : PresetDiscoveryProvider (desc, indexer)
    {
        const auto settingsFile = chowdsp::GlobalPluginSettings::getSettingsFile (chowdsp::toString (settingsFilePath));
        if (settingsFile.existsAsFile())
        {
            try
            {
                const auto settingsJson = chowdsp::JSONUtils::fromFile (settingsFile).at (chowdsp::GlobalPluginSettings::settingsTag);
                userPresetsFolder = settingsJson.at (chowdsp::presets::frontend::SettingsInterface::userPresetsDirID);
                userPresetsLocation = {
                    .flags = CLAP_PRESET_DISCOVERY_IS_USER_CONTENT,
                    .name = "User Presets Location",
                    .kind = CLAP_PRESET_DISCOVERY_LOCATION_FILE,
                    .location = userPresetsFolder.getFullPathName().toRawUTF8(),
                };
            }
            catch (...)
            {
                juce::Logger::writeToLog ("Unable to open settings file!");
                jassertfalse;
            }
        }
    }

    juce::File userPresetsFolder {};
    clap_preset_discovery_location userPresetsLocation {};

    static constexpr clap_preset_discovery_filetype userPresetFiletype {
        .name = "User Preset Filetype",
        .description = "User preset filetype for ChowDSP plugins",
        .file_extension = "chowpreset",
    };

    bool init() noexcept override
    {
        indexer()->declare_filetype (indexer(), &userPresetFiletype);
        if (userPresetsFolder.isDirectory())
            indexer()->declare_location (indexer(), &userPresetsLocation);
        return true;
    }

    bool getMetadata (uint32_t location_kind,
                      const char* location,
                      const clap_preset_discovery_metadata_receiver_t* metadata_receiver) noexcept override
    {
        if (location_kind == CLAP_PRESET_DISCOVERY_LOCATION_PLUGIN || location == nullptr)
            return false;

        const auto userPresetFile = juce::File { location };
        if (! userPresetFile.existsAsFile())
            return false;

        using chowdsp::presets::Preset;
        const Preset preset { userPresetFile };
        if (! preset.isValid())
            return true;

        std::cout << "Indexing user preset: " << preset.getName() << std::endl;
        if (metadata_receiver->begin_preset (metadata_receiver,
                                             preset.getName().toRawUTF8(),
                                             preset.getPresetFile().getFullPathName().toRawUTF8()))
        {
            metadata_receiver->add_plugin_id (metadata_receiver, &plugin_id);
            metadata_receiver->add_creator (metadata_receiver, preset.getVendor().toRawUTF8());

            if (preset.getCategory().isNotEmpty())
                metadata_receiver->add_feature (metadata_receiver, preset.getCategory().toRawUTF8());
        }

        return true;
    }
};

uint32_t count (const clap_preset_discovery_factory*)
{
    return 2;
}

const clap_preset_discovery_provider_descriptor_t* get_descriptor (const clap_preset_discovery_factory*, uint32_t index)
{
    if (index == 0)
        return &FactoryPresetDiscoveryProvider::descriptor;
    if (index == 1)
        return &UserPresetDiscoveryProvider::descriptor;
    return nullptr;
}

const clap_preset_discovery_provider_t* create (const clap_preset_discovery_factory*, const clap_preset_discovery_indexer_t* indexer, const char* provider_id)
{
    if (strcmp (provider_id, FactoryPresetDiscoveryProvider::descriptor.id) == 0)
    {
        auto* provider = new FactoryPresetDiscoveryProvider { &FactoryPresetDiscoveryProvider::descriptor, indexer };
        return provider->provider();
    }

    if (strcmp (provider_id, UserPresetDiscoveryProvider::descriptor.id) == 0)
    {
        auto* provider = new UserPresetDiscoveryProvider { &UserPresetDiscoveryProvider::descriptor, indexer };
        return provider->provider();
    }

    return nullptr;
}

bool presetLoadFromLocation (chowdsp::presets::PresetManager& presetManager,
                             uint32_t location_kind,
                             const char* location,
                             const char* load_key) noexcept
{
    using chowdsp::presets::Preset;
    const auto attemptToLoadPreset = [&presetManager] (const Preset& preset) -> bool
    {
        const auto* presetInTree = presetManager.getPresetTree().findElement (preset);
        if (presetInTree == nullptr)
        {
            // TODO: report error...
            return false;
        }

        presetManager.loadPreset (*presetInTree);
        return true;
    };

    if (location_kind == CLAP_PRESET_DISCOVERY_LOCATION_PLUGIN)
    {
        jassert (location == nullptr);
        juce::Logger::writeToLog ("Host requested load of factory preset: " + juce::String { load_key });
        const auto fs = cmrc::presets::get_filesystem();
        for (auto&& entry : fs.iterate_directory (""))
        {
            const auto preset = state::presets::createPresetFromEmbeddedFile (entry.filename(), fs);
            if (preset.getName() == load_key)
                return attemptToLoadPreset (preset);
        }
    }
    else if (location_kind == CLAP_PRESET_DISCOVERY_LOCATION_FILE)
    {
        const auto presetFile = juce::File { load_key };
        if (presetFile.getFullPathName().isEmpty())
        {
            presetManager.loadDefaultPreset();
            return true;
        }

        juce::Logger::writeToLog ("Host requested load of user preset: " + juce::String { load_key } + ", from location: " + juce::String { location });
        const auto preset = Preset { presetFile };
        if (! preset.isValid())
            return false;
        attemptToLoadPreset (preset);
        return true;
    }

    return false;
}
} // namespace state::presets::discovery

#endif // HAS_CLAP_JUCE_EXTENSIONS
