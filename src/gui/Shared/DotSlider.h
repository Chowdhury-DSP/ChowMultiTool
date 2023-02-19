#pragma once

#include <pch.h>

namespace gui
{
class DotSlider : public juce::Slider
{
public:
    DotSlider (chowdsp::FloatParameter& p, chowdsp::PluginState& state);

    virtual juce::Rectangle<float> getThumbBounds() const noexcept = 0;
    void paint (juce::Graphics& g) override;
    bool hitTest (int x, int y) override;

protected:
    chowdsp::FloatParameter& param;

private:
    chowdsp::SliderAttachment attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DotSlider)
};

class SpectrumDotSlider : public DotSlider
{
public:
    enum Orientation
    {
        FrequencyOriented,
        MagnitudeOriented,
    };

    SpectrumDotSlider (chowdsp::FloatParameter& p,
                       chowdsp::PluginState& state,
                       const chowdsp::SpectrumPlotBase& base,
                       Orientation orientation);

    juce::Rectangle<float> getThumbBounds() const noexcept override;

    double proportionOfLengthToValue (double proportion) override;
    double valueToProportionOfLength (double value) override;

    std::function<float()> getXCoordinate = nullptr;

private:
    const chowdsp::SpectrumPlotBase& plotBase;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumDotSlider)
};
} // namespace gui
