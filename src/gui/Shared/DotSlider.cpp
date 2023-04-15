#include "DotSlider.h"

namespace gui
{
DotSlider::DotSlider (chowdsp::FloatParameter& p, chowdsp::PluginState& state)
    : param (p),
      attachment (param, state, *this)
{
    setTextBoxStyle (juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
}

void DotSlider::paint (juce::Graphics& g)
{
    g.setColour (findColour (thumbColourId));
    g.fillEllipse (getThumbBounds());
}

bool DotSlider::hitTest (int x, int y)
{
    if (! isVisible())
        return false;
    return getThumbBounds().contains ((float) x, (float) y);
}

//==================================================
SpectrumDotSlider::SpectrumDotSlider (chowdsp::FloatParameter& p,
                                      chowdsp::PluginState& state,
                                      const chowdsp::SpectrumPlotBase& base,
                                      Orientation orientation)
    : DotSlider (p, state),
      plotBase (base)
{
    if (orientation == FrequencyOriented)
        setSliderStyle (LinearHorizontal);
    else if (orientation == MagnitudeOriented)
        setSliderStyle (LinearVertical);
}

double SpectrumDotSlider::proportionOfLengthToValue (double proportion)
{
    if (getSliderStyle() == LinearHorizontal)
    {
        const auto xPos = (double) plotBase.getWidth() * proportion;
        return (double) plotBase.getFrequencyForXCoordinate ((float) xPos);
    }
    else if (getSliderStyle() == LinearVertical)
    {
        const auto ret = (double) juce::NormalisableRange { plotBase.params.minMagnitudeDB,
                                                            plotBase.params.maxMagnitudeDB }
                             .convertFrom0to1 ((float) proportion);
        return ret;
    }

    jassertfalse;
    return juce::Slider::proportionOfLengthToValue (proportion);
}

double SpectrumDotSlider::valueToProportionOfLength (double value)
{
    if (getSliderStyle() == LinearHorizontal)
    {
        const auto xPos = (double) plotBase.getXCoordinateForFrequency ((float) value);
        return xPos / (double) plotBase.getWidth();
    }
    else if (getSliderStyle() == LinearVertical)
    {
        const auto yPos = (double) plotBase.getYCoordinateForDecibels ((float) value);
        return yPos / (double) plotBase.getHeight();
    }

    jassertfalse;
    return juce::Slider::valueToProportionOfLength (value);
}

juce::Rectangle<float> SpectrumDotSlider::getThumbBounds() const noexcept
{
    const auto centre = [this]() -> juce::Point<float>
    {
        if (getSliderStyle() == LinearHorizontal)
        {
            if (getYCoordinate == nullptr)
            {
                return juce::Point { plotBase.getXCoordinateForFrequency (param.get()),
                                     plotBase.getYCoordinateForDecibels (0.0f) };
            }
            else
            {
                return juce::Point { plotBase.getXCoordinateForFrequency (param.get()),
                                     getYCoordinate() };
            }
        }
        else if (getSliderStyle() == LinearVertical)
        {
            jassert (getXCoordinate != nullptr);
            return juce::Point { getXCoordinate(),
                                 plotBase.getYCoordinateForDecibels (param.get()) };
        }

        return {};
    }();

    const auto dim = plotBase.getLocalBounds().proportionOfWidth (0.025f);
    return juce::Rectangle { dim, dim }
        .toFloat()
        .withCentre (centre);
}

//==================================================
bool DotSliderGroup::hitTest (int x, int y)
{
    bool result = false;
    for (auto& slider : sliders)
        result |= slider->hitTest (x, y);
    return result;
}

void DotSliderGroup::resized()
{
    for (auto& slider : sliders)
        slider->setBounds (getBoundsInParent());
}

void DotSliderGroup::setSliders (std::vector<DotSlider*>&& newSliders)
{
    sliders = std::move (newSliders);
    for (auto* slider : sliders)
        slider->setInterceptsMouseClicks (false, false);
}

void DotSliderGroup::mouseDown (const juce::MouseEvent& e)
{
    for (auto& slider : sliders)
    {
        slider->toFront (false);
        if (slider->checkModifierKeys (e.mods))
            slider->mouseDown (e);
    }
    toFront (false);
}

void DotSliderGroup::mouseDrag (const juce::MouseEvent& e)
{
    for (auto& slider : sliders)
    {
        if (slider->checkModifierKeys (e.mods))
            slider->mouseDrag (e);
    }
}

void DotSliderGroup::mouseUp (const juce::MouseEvent& e)
{
    for (auto& slider : sliders)
    {
        if (slider->checkModifierKeys (e.mods))
            slider->mouseUp (e);
    }
}

void DotSliderGroup::mouseDoubleClick (const juce::MouseEvent& e)
{
    for (auto& slider : sliders)
    {
        if (slider->checkModifierKeys (e.mods))
            slider->mouseDoubleClick (e);
    }
}
} // namespace gui
