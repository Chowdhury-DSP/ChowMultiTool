#pragma once

#include <pch.h>

namespace state::presets::discovery
{
    uint32_t count (const clap_preset_discovery_factory* factory);

    const clap_preset_discovery_provider_descriptor_t* get_descriptor (const clap_preset_discovery_factory* factory, uint32_t index);

    const clap_preset_discovery_provider_t* create (const clap_preset_discovery_factory* factory, const clap_preset_discovery_indexer_t* indexer, const char* provider_id);

    bool presetLoadFromLocation (chowdsp::presets::PresetManager& presetManager,
                                 uint32_t location_kind,
                                 const char* location,
                                 const char* load_key) noexcept;
} // namespace state::presets
