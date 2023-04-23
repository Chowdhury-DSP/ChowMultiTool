#include "PresetsComponent.h"
#include "gui/Shared/Colours.h"

namespace gui::presets
{
PresetsComponent::PresetsComponent (chowdsp::presets::PresetManager& presetMgr, chowdsp::presets::frontend::FileInterface& fileFace)
    : chowdsp::presets::PresetsComponent (presetMgr, &fileFace)
{
    setColour (juce::Label::backgroundColourId, colours::linesColour.withAlpha (0.5f));

    presetNameDisplay.setFont (juce::Font { fonts->robotoBold }.withHeight (18.0f));
    presetNameEditor.setFont (juce::Font { fonts->robotoBold }.withHeight (18.0f));
}
}
