#include "PresetsFileInterface.h"

namespace gui::presets
{
PresetsFileInterface::PresetsFileInterface (chowdsp::presets::PresetManager& manager,
                                            chowdsp::presets::frontend::SettingsInterface* settingsInterface)
    : FileInterface (manager, settingsInterface),
      presetManager (manager)
{
}

void PresetsFileInterface::deleteCurrentPreset()
{
    const auto* currentPreset = presetManager.getCurrentPreset();
    jassert (currentPreset != nullptr && currentPreset->getPresetFile().existsAsFile() && ! (currentPreset->isFactoryPreset));

    // our custom re-implementation will take care of deleting the preset as well
    checkDeletePresetCallback (*currentPreset);
}
}
