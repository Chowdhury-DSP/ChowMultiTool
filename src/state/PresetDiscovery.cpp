#if HAS_CLAP_JUCE_EXTENSIONS

#include "PresetDiscovery.h"

JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wunused-parameter")
#include <clap/helpers/preset-discovery-provider.hh>
#include <clap/helpers/preset-discovery-provider.hxx>
JUCE_END_IGNORE_WARNINGS_GCC_LIKE

namespace state::presets::discovery
{
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
        .name = "Binary Data",
        .kind = CLAP_PRESET_DISCOVERY_LOCATION_PLUGIN,
        .location = nullptr,
    };

    bool init() noexcept override
    {
        indexer()->declare_location (indexer(), &factoryPresetsLocation);
        return true;
    }

    static constexpr clap_plugin_id plugin_id {
        .abi = "clap",
        .id = "org.chowdsp.ChowMultiTool" // TODO: get this in an automated way...
    };

    bool getMetadata (uint32_t location_kind,
                      const char* location,
                      const clap_preset_discovery_metadata_receiver_t* metadata_receiver) noexcept override
    {
        if (location_kind != CLAP_PRESET_DISCOVERY_LOCATION_PLUGIN || location != nullptr)
            return false;

        using chowdsp::presets::Preset;
        const auto fs = cmrc::presets::get_filesystem();
        const auto createPresetFromEmbeddedFile = [&fs] (const std::string& path) -> Preset
        {
            const auto presetFile = fs.open (path);
            return { presetFile.begin(), presetFile.size() };
        };

        for (auto&& entry : fs.iterate_directory (""))
        {
            jassert (entry.is_file());
            jassert (fs.exists (entry.filename()));
            const auto factoryPreset = createPresetFromEmbeddedFile (entry.filename());

            std::cout << "Indexing preset: " << factoryPreset.getName() << std::endl;
            if (metadata_receiver->begin_preset (metadata_receiver, factoryPreset.getName().toRawUTF8(), "load-key"))
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

uint32_t count (const clap_preset_discovery_factory*)
{
    return 1;
}

const clap_preset_discovery_provider_descriptor_t* get_descriptor (const clap_preset_discovery_factory*, uint32_t index)
{
    if (index == 0)
        return &FactoryPresetDiscoveryProvider::descriptor;
    return nullptr;
}

const clap_preset_discovery_provider_t* create (const clap_preset_discovery_factory*, const clap_preset_discovery_indexer_t* indexer, const char* provider_id)
{
    if (strcmp (provider_id, FactoryPresetDiscoveryProvider::descriptor.id) == 0)
    {
        auto* provider = new FactoryPresetDiscoveryProvider { &FactoryPresetDiscoveryProvider::descriptor, indexer };
        return provider->provider();
    }

    return nullptr;
}
} // namespace state::presets::discovery

#endif // HAS_CLAP_JUCE_EXTENSIONS
