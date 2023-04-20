#pragma once

#include <pch.h>

#if ! JUCE_IOS
#include "clap-juce-extensions/clap-juce-extensions.h"

namespace state
{
struct PluginParams;
struct RemoteControlsHelper
{
    RemoteControlsHelper (chowdsp::PluginState& state, PluginParams& params, clap_juce_extensions::clap_juce_audio_processor_capabilities* cje);

    static int getNumPages();

    void pageFill (uint32_t pageIndex,
                   juce::String& sectionName,
                   uint32_t& pageID,
                   juce::String& pageName,
                   std::array<juce::AudioProcessorParameter*, CLAP_REMOTE_CONTROLS_COUNT>& params);

private:
    PluginParams& params;
    chowdsp::ScopedCallback toolChangeCallback;
};
} // namespace state
#endif // ! JUCE_IOS
