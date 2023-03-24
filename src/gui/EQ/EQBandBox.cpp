#include "EQBandBox.h"
#include "dsp/EQ/EQProcessor.h"

namespace gui::eq
{
BandBoxAttachment::BandBoxAttachment (chowdsp::BoolParameter& onOffParam,
                                      chowdsp::ChoiceParameter& typeParam,
                                      chowdsp::PluginState& state,
                                      juce::ComboBox& combo)
    : comboBox (combo),
      onOffAttachment (onOffParam, state.getParameterListeners(), [this] (bool v)
                       { setOnOffValue (v); }),
      typeAttachment (typeParam, state.getParameterListeners(), [this] (int v)
                      { setTypeValue (v); }),
      um (state.undoManager)
{
    onTypeIndex = typeParam.getIndex() + 1;
    combo.setSelectedItemIndex (onOffParam.get() == false ? 0 : onTypeIndex);
    comboBox.addListener (this);
}

BandBoxAttachment::~BandBoxAttachment()
{
    comboBox.removeListener (this);
}

void BandBoxAttachment::setOnOffValue (bool newValue)
{
    juce::ScopedValueSetter svs { skipBoxChangedCallback, true };
    comboBox.setSelectedItemIndex (newValue == false ? 0 : onTypeIndex, juce::sendNotificationSync);
}

void BandBoxAttachment::setTypeValue (int newValue)
{
    juce::ScopedValueSetter svs { skipBoxChangedCallback, true };
    comboBox.setSelectedItemIndex (newValue + 1, juce::sendNotificationSync);
    onTypeIndex = newValue + 1;
}

void BandBoxAttachment::comboBoxChanged (juce::ComboBox*)
{
    if (skipBoxChangedCallback)
        return;

    const auto newValue = comboBox.getSelectedItemIndex();
    if (newValue == 0)
    {
        onOffAttachment.setValueAsCompleteGesture (false, um);
    }
    else
    {
        onTypeIndex = newValue;
        typeAttachment.setValueAsCompleteGesture (newValue - 1, um);
        onOffAttachment.setValueAsCompleteGesture (true, um);
    }
}

EQBandBox::EQBandBox()
{
    auto choices = dsp::eq::EQToolParams::bandTypeChoices;
    choices.insert (0, "Off");
    addItemList (choices, 1);
}

void EQBandBox::initialise (chowdsp::PluginState& state, chowdsp::BoolParameter& onOffParam, chowdsp::ChoiceParameter& typeParam)
{
    attachment.emplace (onOffParam, typeParam, state, *this);
}
} // namespace gui::eq
