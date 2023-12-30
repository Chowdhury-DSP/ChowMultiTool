#pragma once

#include "BandSplitterPlot.h"
//#include "SlopePicker.h"
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
    dsp::band_splitter::ExtraState& extraState;

    struct BandsButton : juce::Button
    {
        BandsButton (chowdsp::BoolParameter& param, State& pluginState);
        void paintButton (juce::Graphics& g, bool, bool) override;
        chowdsp::ButtonAttachment attach;
        chowdsp::BoolParameter& bandParam;
        gui::SharedFonts fonts;
    } bandsButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandSplitterEditor)
};
} // namespace gui::band_splitter
