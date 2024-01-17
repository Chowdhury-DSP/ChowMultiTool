#include "TriStateButtonAttachment.h"
#include "BandSplitterEditor.h"
#include <pch.h>

namespace gui::band_splitter
{
TriStateButtonAttachment::TriStateButtonAttachment (chowdsp::BoolParameter& threeBandParam,
                                                    chowdsp::BoolParameter& fourBandParam,
                                                    chowdsp::PluginState& pluginState,
                                                    juce::Button& triStateButton,
                                                    std::pair<BandState, int>& currentState)
    : threeBandAttachment(threeBandParam, pluginState, [this](bool threeBandOn) { updateButtonState(); }),
      fourBandAttachment(fourBandParam, pluginState, [this](bool fourBandOn) { updateButtonState(); }),
      bandStateButton (&triStateButton),
      um(pluginState.undoManager),
      currentState (currentState),
      threeBandParam (threeBandParam),
      fourBandParam (fourBandParam)
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
        threeBandParam.setValueNotifyingHost (true);
        fourBandParam.setValueNotifyingHost (false);
    }
    else if (currentState.first == BandState::ThreeBands)
    {
        threeBandParam.setValueNotifyingHost (true);
        fourBandParam.setValueNotifyingHost (true);
    }
    else if (currentState.first == BandState::FourBands)
    {
        threeBandParam.setValueNotifyingHost (false);
        fourBandParam.setValueNotifyingHost (false);
    }

    threeBandAttachment.setValueAsCompleteGesture(threeBandParam.get(), um);
    fourBandAttachment.setValueAsCompleteGesture(fourBandParam.get(), um);
}
} // namespace gui::band_splitter