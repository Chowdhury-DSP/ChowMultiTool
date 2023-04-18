#pragma once

#include <pch.h>

class ChowMultiTool;
namespace gui
{
class SettingsButton : public juce::DrawableButton
{
public:
    SettingsButton (ChowMultiTool& plugin, chowdsp::OpenGLHelper& oglHelper);

private:
    void showSettingsMenu();
    void addPluginSettingMenuOption (const juce::String& name, const chowdsp::GlobalPluginSettings::SettingID& id, juce::PopupMenu& menu, int itemID);

    ChowMultiTool& plugin;
    chowdsp::OpenGLHelper& oglHelper;

    chowdsp::SharedPluginSettings pluginSettings;
    chowdsp::SharedLNFAllocator lnfAllocator;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsButton)
};
}
