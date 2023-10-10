#pragma once

#include <pch.h>
#include "Toolbar/Toolbar.h"
#include "gui/Shared/ErrorMessageView.h"
#include "dsp/Shared/EQHelpers.h"
//#include "chowdsp_TupleHelpers.h"

class ChowMultiTool;

namespace gui
{
class PluginEditor : public juce::AudioProcessorEditor,
                     public chowdsp::TrackedByBroadcasters
{
public:
    explicit PluginEditor (ChowMultiTool& plugin);
    ~PluginEditor() override;

    EQHelpers& getProcessorHelper();
    void paint (juce::Graphics& g) override;
    void resized() override;

    auto& getErrorMessageView() { return errorMessageView; }

    static constexpr chowdsp::GlobalPluginSettings::SettingID openGLSettingID { "use_opengl" };

private:
    void mouseDoubleClick (const juce::MouseEvent&) override;

    void setResizeBehaviour();
    void refreshEditor();

    ChowMultiTool& plugin;
    chowdsp::HostContextProvider hostContextProvider;

    Toolbar toolbar;
    std::unique_ptr<juce::Component> editorComponent;
    ErrorMessageView errorMessageView;

    chowdsp::ScopedCallback toolChangeCallback;
    juce::ComponentBoundsConstrainer constrainer;
    chowdsp::SharedLNFAllocator lnfAllocator;

    chowdsp::SharedPluginSettings pluginSettings;
    chowdsp::OpenGLHelper oglHelper;
    void openGLChangeCallback (chowdsp::GlobalPluginSettings::SettingID settingID);

#if JUCE_MODULE_AVAILABLE_melatonin_inspector
    melatonin::Inspector inspector { *this, false };
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
} // namespace gui
