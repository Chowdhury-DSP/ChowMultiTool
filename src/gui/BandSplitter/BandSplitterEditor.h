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
                        std::pair<gui::SpectrumAnalyserTask::Optional, gui::SpectrumAnalyserTask::Optional> spectrumAnalyserTasks);

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

    std::vector<std::unique_ptr<SpectrumAnalyser>> spectrumAnalysers; //push_back instances of SpectrumAnalyser and destroy as needed

    //develop a pattern whereby multiple instances of SpectrumAnalyser can be managed dependent on the number of bands
    //perhaps a dynamic container such as std::vector??

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandSplitterEditor)
};
} // namespace gui::band_splitter
