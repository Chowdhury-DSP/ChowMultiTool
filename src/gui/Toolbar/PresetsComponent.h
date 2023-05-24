#pragma once

#include "gui/Shared/Fonts.h"
#include "PresetsFileInterface.h"

namespace gui::presets
{
class PresetsComponent : public chowdsp::presets::PresetsComponent
{
public:
    PresetsComponent (chowdsp::presets::PresetManager& presetManager, PresetsFileInterface& fileInterface);

    bool queryShouldDeletePreset (const chowdsp::presets::Preset& preset) override;

private:
    SharedFonts fonts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetsComponent)
};
} // namespace gui::presets
