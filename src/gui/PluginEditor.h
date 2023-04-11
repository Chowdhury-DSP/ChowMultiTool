#pragma once

#include "Toolbar/Toolbar.h"
#include "gui/Shared/ErrorMessageView.h"

class ChowMultiTool;

namespace gui
{
class PluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginEditor (ChowMultiTool& plugin);
    ~PluginEditor() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    auto& getErrorMessageView() { return errorMessageView; }

private:
    void mouseDoubleClick (const juce::MouseEvent&) override;

    void setResizeBehaviour();
    void refreshEditor();

    ChowMultiTool& plugin;

    Toolbar toolbar;
    std::unique_ptr<juce::Component> editorComponent;
    ErrorMessageView errorMessageView;

    chowdsp::ScopedCallback toolChangeCallback;
    juce::ComponentBoundsConstrainer constrainer;
    chowdsp::SharedLNFAllocator lnfAllocator;

    melatonin::Inspector inspector { *this, false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
} // namespace gui
