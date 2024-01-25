#include "TriStateButtonAttachment.h"
#include "BandSplitterEditor.h"
#include <pch.h>

namespace gui::band_splitter
{
TriStateButtonAttachment::TriStateButtonAttachment (State& pluginState,
                                                    juce::Button& triStateButton,
                                                    std::pair<BandState, int>& currentState)
    : threeBandParam (pluginState.params.bandSplitParams->threeBandOnOff),
      fourBandParam (pluginState.params.bandSplitParams->fourBandOnOff),
      threeBandAttachment (threeBandParam, pluginState, [this] (bool threeBandOn)
                           { updateButtonState(); }),
      fourBandAttachment (fourBandParam, pluginState, [this] (bool fourBandOn)
                          { updateButtonState(); }),
      bandStateButton (&triStateButton),
      um (pluginState.undoManager),
      currentState (currentState)
{
    updateButtonState();
    bandStateButton->addListener (this);
}

TriStateButtonAttachment::~TriStateButtonAttachment()
{
    if (bandStateButton != nullptr)
        bandStateButton->removeListener (this);
    updateButtonState();
}

void TriStateButtonAttachment::updateButtonState()
{
    if (! threeBandParam.get() && ! fourBandParam.get())
        currentState = std::make_pair (BandState::TwoBands, 2);
    else if (threeBandParam.get() && ! fourBandParam.get())
        currentState = std::make_pair (BandState::ThreeBands, 3);
    else
        currentState = std::make_pair (BandState::FourBands, 4);
    bandStateButton->repaint();
}

void TriStateButtonAttachment::setParametersFromUI (BandState newBandState)
{
    auto setParameter = [this] (chowdsp::BoolParameter& param, bool newValue)
    {
        if (param.get() == newValue)
            return;

        um->perform (
            new chowdsp::ParameterAttachmentHelpers::ParameterChangeAction (
                param,
                chowdsp::ParameterTypeHelpers::getValue (param),
                newValue));

        param.beginChangeGesture();
        chowdsp::ParameterTypeHelpers::setValue (newValue, param);
        param.endChangeGesture();
    };

    um->beginNewTransaction();
    if (newBandState == BandState::TwoBands)
    {
        setParameter (*threeBandAttachment.param, false);
        setParameter (*fourBandAttachment.param, false);
    }
    else if (newBandState == BandState::ThreeBands)
    {
        setParameter (*threeBandAttachment.param, true);
        setParameter (*fourBandAttachment.param, false);
    }
    else if (newBandState == BandState::FourBands)
    {
        setParameter (*threeBandAttachment.param, true);
        setParameter (*fourBandAttachment.param, true);
    }
}

void TriStateButtonAttachment::buttonClicked (juce::Button* button)
{
    if (button != bandStateButton)
        return;

    if (currentState.first == BandState::TwoBands)
    {
        setParametersFromUI (BandState::ThreeBands);
    }
    else if (currentState.first == BandState::ThreeBands)
    {
        setParametersFromUI (BandState::FourBands);
    }
    else if (currentState.first == BandState::FourBands)
    {
        setParametersFromUI (BandState::TwoBands);
    }
}
} // namespace gui::band_splitter