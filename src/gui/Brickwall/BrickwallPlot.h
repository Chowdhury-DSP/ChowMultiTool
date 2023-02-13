#pragma once

#include "dsp/Brickwall/BrickwallProcessor.h"
#include "state/PluginState.h"

namespace gui::brickwall
{
class BrickwallPlot : public chowdsp::SpectrumPlotBase
{
public:
    BrickwallPlot (State& pluginState, dsp::brickwall::Params& params);

    void paint (juce::Graphics& g) override;
    void paintOverChildren (juce::Graphics& g) override;
    void resized() override;

private:
    void updatePlot();

    chowdsp::GenericFilterPlotter filterPlotter;
    dsp::brickwall::BrickwallProcessor brickwall;

    chowdsp::ScopedCallbackList callbacks;

    struct InternalSlider : juce::Slider
    {
        InternalSlider (chowdsp::FloatParameter& cutoffParam,
                        chowdsp::SpectrumPlotBase& plotBase,
                        State& pluginState);
        void paint (juce::Graphics& g) override;
        bool hitTest (int x, int y) override;
        double proportionOfLengthToValue (double proportion) override;
        double valueToProportionOfLength (double value) override;
        juce::Rectangle<int> getThumbBounds() const;

        chowdsp::FloatParameter& cutoffParam;
        chowdsp::SpectrumPlotBase& plotBase;
        chowdsp::SliderAttachment cutoffAttachment;
    } cutoffSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BrickwallPlot)
};
} // namespace gui::brickwall
