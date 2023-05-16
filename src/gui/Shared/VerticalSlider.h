#pragma once

#include <pch.h>

namespace gui
{
class VerticalSlider : public juce::Slider,
                       private juce::Timer
{
public:
    VerticalSlider (const chowdsp::FloatParameter& param, const chowdsp::HostContextProvider& hostContextProvider);

    void paint (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& e) override;
    auto& getParameter() const { return param; }

protected:
    void timerCallback() override;
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float modSliderPos);

    const chowdsp::FloatParameter& param;
    const chowdsp::HostContextProvider& hostContextProvider;

    double modulatedValue = param.getCurrentValue();

    struct KnobAssets
    {
        std::unique_ptr<juce::Drawable> knob = juce::Drawable::createFromImageData (chowdsp_BinaryData::knob_svg, chowdsp_BinaryData::knob_svgSize);
        std::unique_ptr<juce::Drawable> pointer = juce::Drawable::createFromImageData (chowdsp_BinaryData::pointer_svg, chowdsp_BinaryData::pointer_svgSize);
    };
    juce::SharedResourcePointer<KnobAssets> sharedAssets;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VerticalSlider)
};
} // namespace gui
