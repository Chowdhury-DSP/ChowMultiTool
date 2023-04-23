#pragma once

#include "dsp/BandSplitter/BandSplitterProcessor.h"
#include "state/PluginState.h"

namespace gui::band_splitter
{
class BandSplitterPlot : public chowdsp::EQ::EqualizerPlot
{
public:
    BandSplitterPlot (State& pluginState, dsp::band_splitter::Params& params, const chowdsp::HostContextProvider& hcp);

    void paintOverChildren (juce::Graphics& g) override;
    void resized() override;

private:
    void updateCutoffFrequency();
    void updateFilterSlope();

    const dsp::band_splitter::Params& bandSplitterParams;
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
    } cutoffSlider, cutoff2Slider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandSplitterPlot)
};
} // namespace gui::band_splitter
