#pragma once

#include "EQChyron.h"
#include "EQDrawView.h"
#include "gui/Shared/DotSlider.h"
#include "state/PluginState.h"

namespace gui::eq
{

class EQPlot : public chowdsp::EQ::EqualizerPlotWithParameters<numBands>
{
public:
    EQPlot (chowdsp::PluginState& pluginState,
            chowdsp::EQ::StandardEQParameters<numBands>& eqParameters,
            const chowdsp::HostContextProvider& hcp);

    void paint (juce::Graphics& g) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent& e) override;
    void toggleDrawView (bool shouldShowDrawView, bool triggerOptimiser);

private:
    void setSelectedBand (int bandIndex);

    struct QDotSlider : SpectrumDotSlider
    {
        using SpectrumDotSlider::SpectrumDotSlider;
        juce::Rectangle<float> getThumbBounds() const noexcept override;
        double proportionOfLengthToValue (double proportion) override;
        double valueToProportionOfLength (double value) override;
    };

    struct EQBandSliderGroup : DotSliderGroup
    {
        bool isSelected = false;
        void paint (juce::Graphics& g) override;
    };

    std::array<std::optional<SpectrumDotSlider>, numBands> freqSliders;
    std::array<std::optional<SpectrumDotSlider>, numBands> gainSliders;
    std::array<std::optional<QDotSlider>, numBands> qSliders;
    std::array<EQBandSliderGroup, numBands> sliderGroups;

    EQChyron chyron;

    EQDrawView drawView;
    bool drawMode = false;

    chowdsp::ScopedCallbackList callbacks;
    chowdsp::PluginState& state;
    chowdsp::EQ::StandardEQParameters<numBands>& eqParameters;

    struct IterationsLabel : juce::Label, juce::Timer
    {
        explicit IterationsLabel (EQDrawView& view);
        void visibilityChanged() override;
        void timerCallback() override;
        const EQDrawView& drawView;
        chowdsp::ScopedCallback optimiserFinishedCallback;
    };
    IterationsLabel optItersLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQPlot)
};
} // namespace gui::eq
