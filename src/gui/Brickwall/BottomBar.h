#pragma once

#include "gui/Shared/SimpleBox.h"
#include "state/PluginState.h"

namespace gui::brickwall
{
class BottomBar : public juce::Component
{
public:
    BottomBar (State& pluginState, dsp::brickwall::Params& params);

    void resized() override;
    void paint (juce::Graphics& g) override;

private:
    SimpleBox modeMenu;
    SimpleBox filterTypeMenu;
    SimpleBox slopeMenu;

    chowdsp::ComboBoxAttachment modeAttach;
    chowdsp::ComboBoxAttachment filterTypeAttach;
    chowdsp::ComboBoxAttachment slopeAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BottomBar)
};
} // namespace gui::brickwall
