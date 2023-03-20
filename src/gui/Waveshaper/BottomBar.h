#pragma once

#include "gui/Shared/SimpleBox.h"
#include "state/PluginState.h"

namespace gui::waveshaper
{
class BottomBar : public juce::Component
{
public:
    BottomBar (State& pluginState, dsp::waveshaper::Params& wsParams);

    void resized() override;
    void paint (juce::Graphics& g) override;

private:
    SimpleBox shapeMenu;
    SimpleBox oversampleMenu;

    chowdsp::ComboBoxAttachment shapeAttach;
    chowdsp::ComboBoxAttachment osAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BottomBar)
};
} // namespace gui::waveshaper
