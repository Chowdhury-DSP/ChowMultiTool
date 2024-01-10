#include <pch.h>
#include "TriStateButtonAttachment.h"
#include "BandSplitterEditor.h"

namespace gui::band_splitter
{
TriStateButtonAttachment::TriStateButtonAttachment (chowdsp::BoolParameter& threeBandParam,
                                                    chowdsp::BoolParameter& fourBandParam,
                                                    juce::Button& triStateButton,
                                                    std::pair<BandState, int>& currentState)
    : bandStateButton (&triStateButton), currentState (currentState), threeBandParam (threeBandParam), fourBandParam (fourBandParam)
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
}

void TriStateButtonAttachment::buttonClicked (juce::Button* button)
{
    if (button != bandStateButton)
        return;

    if (currentState.first == BandState::TwoBands)
    {
        currentState = std::make_pair (BandState::ThreeBands, 3);
        threeBandParam.setValueNotifyingHost (true);
        fourBandParam.setValueNotifyingHost (false);
    }
    else if (currentState.first == BandState::ThreeBands)
    {
        currentState = std::make_pair (BandState::FourBands, 4);
        threeBandParam.setValueNotifyingHost (true);
        fourBandParam.setValueNotifyingHost (true);
    }
    else if (currentState.first == BandState::FourBands)
    {
        currentState = std::make_pair (BandState::TwoBands, 2);
        threeBandParam.setValueNotifyingHost (false);
        fourBandParam.setValueNotifyingHost (false);
    }
}
} // namespace gui::band_splitter