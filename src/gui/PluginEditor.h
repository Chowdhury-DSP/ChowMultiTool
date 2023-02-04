#pragma once

#include "ChowMultiTool.h"
#include "Toolbar/Toolbar.h"

namespace gui
{
class PluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginEditor (ChowMultiTool& plugin);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void setResizeBehaviour();
    void refreshEditor();

    ChowMultiTool& plugin;

    Toolbar toolbar;
    std::unique_ptr<juce::Component> editorComponent;

    chowdsp::ScopedCallback toolChangeCallback;
    juce::ComponentBoundsConstrainer constrainer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
} // namespace gui
