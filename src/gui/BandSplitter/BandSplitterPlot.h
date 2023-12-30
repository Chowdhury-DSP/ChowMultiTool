#pragma once

#include "dsp/BandSplitter/BandSplitterProcessor.h"
#include "gui/Shared/SpectrumAnalyser.h"
#include "state/PluginState.h"

namespace gui::band_splitter
{
class BandSplitterPlot : public chowdsp::EQ::EqualizerPlot
{
public:
    BandSplitterPlot (State& pluginState,
                      dsp::band_splitter::Params& params,
                      dsp::band_splitter::ExtraState& bandSplitterExtraState,
                      const chowdsp::HostContextProvider& hcp,
                      dsp::band_splitter::BandSplitterSpectrumTasks& spectrumTasks);
    ~BandSplitterPlot() override;

    void paintOverChildren (juce::Graphics& g) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent& event) override;
    void setSpectrumColours();

private:
    void updateCutoffFrequency();
    void updateFilterSlope();
    void updateSpectrumPlots();

    const dsp::band_splitter::Params& bandSplitterParams;
    dsp::band_splitter::ExtraState& extraState;
    chowdsp::ScopedCallbackList callbacks;

    struct InternalSlider : juce::Slider
    {
        InternalSlider (chowdsp::FloatParameter& cutoffParam,
                        chowdsp::EQ::EqualizerPlot& plotBase,
                        State& pluginState,
                        const chowdsp::HostContextProvider& hcp);
        void paint (juce::Graphics& g) override;
        bool hitTest (int x, int y) override;
        void mouseDown (const juce::MouseEvent& e) override;
        double proportionOfLengthToValue (double proportion) override;
        double valueToProportionOfLength (double value) override;
        juce::Rectangle<int> getThumbBounds() const;

        chowdsp::FloatParameter& cutoffParam;
        chowdsp::EQ::EqualizerPlot& plotBase;
        chowdsp::SliderAttachment cutoffAttachment;
        const chowdsp::HostContextProvider& hostContextProvider;
    } cutoffSlider, cutoff2Slider; //add another slider here for 4 band

    dsp::band_splitter::BandSplitterSpectrumTasks& spectrumTasks;
    chowdsp::SmallMap<dsp::band_splitter::SpectrumBandID, std::unique_ptr<SpectrumAnalyser>> spectrumAnalysers;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandSplitterPlot)
};
} // namespace gui::band_splitter
