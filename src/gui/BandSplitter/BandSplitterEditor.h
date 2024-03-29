#pragma once

#include "BandSplitterPlot.h"
#include "TriStateButtonAttachment.h"
#include "gui/Shared/ChoicePicker.h"
#include "gui/Shared/SpectrumAnalyser.h"

namespace gui::band_splitter
{
class BandSplitterEditor : public juce::Component
{
public:
    BandSplitterEditor (State& pluginState,
                        dsp::band_splitter::Params& params,
                        dsp::band_splitter::ExtraState& bandSplitterExtraState,
                        const chowdsp::HostContextProvider& hcp,
                        dsp::band_splitter::BandSplitterSpectrumTasks& spectrumTasks);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    BandSplitterPlot bandSplitterPlot;
    ChoicePicker<dsp::band_splitter::Slope> slopePicker;

    struct TriStateButton : public juce::Button
    {
        TriStateButton (State& pluginState);
        void paintButton (juce::Graphics& g, bool, bool) override;

        gui::SharedFonts fonts;
        TriStateButtonAttachment triStateButtonAttachment;
        std::pair<BandState, int> currentState;
    } triStateButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandSplitterEditor)
};
} // namespace gui::band_splitter
