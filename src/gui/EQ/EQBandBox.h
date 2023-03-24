#pragma once

#include "gui/Shared/SimpleBox.h"

namespace gui::eq
{
class BandBoxAttachment : private juce::ComboBox::Listener
{
public:
    BandBoxAttachment (chowdsp::BoolParameter& onOffParam,
                       chowdsp::ChoiceParameter& typeParam,
                       chowdsp::PluginState& pluginState,
                       juce::ComboBox& combo);
    ~BandBoxAttachment() override;

    void setOnOffValue (bool newValue);
    void setTypeValue (int newValue);

private:
    void comboBoxChanged (juce::ComboBox*) override;

    juce::ComboBox& comboBox;
    chowdsp::ParameterAttachment<chowdsp::BoolParameter> onOffAttachment;
    chowdsp::ParameterAttachment<chowdsp::ChoiceParameter> typeAttachment;
    juce::UndoManager* um = nullptr;

    bool skipBoxChangedCallback = false;
    int onTypeIndex = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandBoxAttachment)
};

class EQBandBox : public SimpleBox
{
public:
    EQBandBox();

    void initialise (chowdsp::PluginState& state, chowdsp::BoolParameter& onOffParam, chowdsp::ChoiceParameter& typeParam);

private:
    std::optional<BandBoxAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQBandBox)
};
} // namespace gui::eq
