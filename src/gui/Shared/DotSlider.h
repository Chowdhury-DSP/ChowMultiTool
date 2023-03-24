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

    std::function<bool (const juce::ModifierKeys&)> checkModifierKeys = [] (const juce::ModifierKeys& mods)
    { return ! mods.isAnyModifierKeyDown(); };

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
    std::function<float()> getYCoordinate = nullptr;

    const chowdsp::SpectrumPlotBase& plotBase;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumDotSlider)
};

struct DotSliderGroup : public juce::Component
{
    DotSliderGroup() = default;

    bool hitTest (int x, int y) override;

    void resized() override;

    void setSliders (std::vector<DotSlider*>&& sliders);

    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override;
    void mouseDoubleClick (const juce::MouseEvent&) override;

    std::vector<DotSlider*> sliders;
};
} // namespace gui
