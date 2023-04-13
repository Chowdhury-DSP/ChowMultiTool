#pragma once

#include "gui/Shared/SimpleBox.h"
#include "state/PluginState.h"

namespace dsp::svf
{
enum class SVFType;
}

namespace gui::svf
{
class BottomBar : public juce::Component
{
public:
    BottomBar (State& pluginState, dsp::svf::Params& svfParams);

    void resized() override;
    void paint (juce::Graphics& g) override;

private:
    void updateVisibilities (dsp::svf::SVFType svfType);

    SimpleBox filterTypeMenu;
    SimpleBox plainFilterSubTypeMenu;
    SimpleBox arpFilterSubTypeMenu;
    SimpleBox wernerFilterSubTypeMenu;

    chowdsp::ComboBoxAttachment filterTypeAttach;
    chowdsp::ComboBoxAttachment plainSubTypeAttach;
    chowdsp::ComboBoxAttachment arpSubTypeAttach;
    chowdsp::ComboBoxAttachment wernerSubTypeAttach;

    chowdsp::ScopedCallback typeChangeCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BottomBar)
};
} // namespace gui::svf
