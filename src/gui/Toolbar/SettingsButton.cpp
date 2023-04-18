#include "SettingsButton.h"
#include "ChowMultiTool.h"
#include "gui/PluginEditor.h"
#include "gui/Shared/LookAndFeels.h"
#include "gui/Shared/Colours.h"

namespace gui
{
SettingsButton::SettingsButton (ChowMultiTool& plug, chowdsp::OpenGLHelper& openglHelper)
    : DrawableButton ("Settings", juce::DrawableButton::ImageFitted),
      plugin (plug),
      oglHelper (openglHelper)
{
    const auto fs = cmrc::gui::get_filesystem();
    const auto cogSVGFile = fs.open ("Vector/cog-solid.svg");
    setImages (juce::Drawable::createFromImageData (cogSVGFile.begin(), cogSVGFile.size()).get());
    onClick = [this]
    { showSettingsMenu(); };
}

void SettingsButton::showSettingsMenu()
{
    juce::PopupMenu menu;

    if (oglHelper.isOpenGLAvailable())
        addPluginSettingMenuOption ("Use OpenGL", PluginEditor::openGLSettingID, menu, 200);

    menu.addSeparator();
    menu.addItem ("View Source Code", []
                  { juce::URL ("https://github.com/Chowdhury-DSP/ChowMultiTool").launchInDefaultBrowser(); });

    menu.addItem ("Copy Diagnostic Info",
                  [this]
                  {
                      juce::Logger::writeToLog ("Copying diagnostic info...");
                      juce::SystemClipboard::copyTextToClipboard (chowdsp::PluginDiagnosticInfo::getDiagnosticsString (plugin));
                  });

    auto options = juce::PopupMenu::Options()
                       .withParentComponent (getParentComponent()->getParentComponent())
                       .withPreferredPopupDirection (juce::PopupMenu::Options::PopupDirection::downwards)
                       .withStandardItemHeight (27);
    menu.setLookAndFeel (lnfAllocator->getLookAndFeel<gui::lnf::MenuLNF>());
    menu.showMenuAsync (options);
}

void SettingsButton::addPluginSettingMenuOption (const juce::String& name,
                                                 const chowdsp::GlobalPluginSettings::SettingID& id,
                                                 juce::PopupMenu& menu,
                                                 int itemID)
{
    const auto isCurrentlyOn = pluginSettings->getProperty<bool> (id);

    juce::PopupMenu::Item item;
    item.itemID = itemID;
    item.text = name;
    item.action = [this, id, isCurrentlyOn]
    { pluginSettings->setProperty (id, ! isCurrentlyOn); };
    item.colour = isCurrentlyOn ? colours::linesColour : logo::colours::accentRed;

    menu.addItem (item);
}
} // namespace gui
