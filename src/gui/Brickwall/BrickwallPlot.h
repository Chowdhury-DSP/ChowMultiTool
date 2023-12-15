#pragma once

#include "BrickwallChyron.h"
#include "dsp/Brickwall/BrickwallProcessor.h"
#include "gui/Shared/SpectrumAnalyser.h"
#include "state/PluginState.h"

namespace gui::brickwall
{
class BrickwallPlot : public chowdsp::SpectrumPlotBase
{
public:
    BrickwallPlot (State& pluginState,
                   dsp::brickwall::Params& params,
                   dsp::brickwall::ExtraState& brickwallExtraState,
                   const chowdsp::HostContextProvider& hcp,
                   std::pair<gui::SpectrumAnalyserTask::Optional, gui::SpectrumAnalyserTask::Optional> spectrumAnalyserTasks);

    ~BrickwallPlot();
    void paint (juce::Graphics& g) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent& event) override;

private:
    void updatePlot();

    struct FilterPlotComp : juce::Component
    {
        void paint (juce::Graphics& g);
        void resized() { repaint(); }

        BrickwallPlot* parent = nullptr;
    };

    chowdsp::GenericFilterPlotter filterPlotter;
    FilterPlotComp plotComp;
    dsp::brickwall::BrickwallProcessor brickwall;
    dsp::brickwall::ExtraState& extraState;

    chowdsp::ScopedCallbackList callbacks;

    SpectrumAnalyser spectrumAnalyser;
    BrickwallChyron chyron;

    struct InternalSlider : juce::Slider
    {
        InternalSlider (chowdsp::FloatParameter& cutoffParam,
                        chowdsp::SpectrumPlotBase& plotBase,
                        State& pluginState,
                        const chowdsp::HostContextProvider& hcp);
        void paint (juce::Graphics& g) override;
        bool hitTest (int x, int y) override;
        void mouseDown (const juce::MouseEvent& e) override;
        double proportionOfLengthToValue (double proportion) override;
        double valueToProportionOfLength (double value) override;
        juce::Rectangle<int> getThumbBounds() const;

        chowdsp::FloatParameter& cutoffParam;
        chowdsp::SpectrumPlotBase& plotBase;
        chowdsp::SliderAttachment cutoffAttachment;
        const chowdsp::HostContextProvider& hostContextProvider;
    } cutoffSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BrickwallPlot)
};
} // namespace gui::brickwall
