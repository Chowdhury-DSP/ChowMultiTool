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

void TriStateButtonAttachment::buttonClicked (juce::Button* button)
{
    if (button != bandStateButton)
        return;

    if (currentState.first == BandState::TwoBands)
    {
        threeBandAttachment.setValueAsCompleteGesture (true, um);
        fourBandAttachment.setValueAsCompleteGesture (false, um);
    }
    else if (currentState.first == BandState::ThreeBands)
    {
        threeBandAttachment.setValueAsCompleteGesture (true, um);
        fourBandAttachment.setValueAsCompleteGesture (true, um);
    }
    else if (currentState.first == BandState::FourBands)
    {
        threeBandAttachment.setValueAsCompleteGesture (false, um);
        fourBandAttachment.setValueAsCompleteGesture (false, um);
    }
}
} // namespace gui::band_splitter