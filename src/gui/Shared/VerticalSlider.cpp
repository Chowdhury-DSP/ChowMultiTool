#include "VerticalSlider.h"
#include "LookAndFeels.h"
#include "Colours.h"

namespace gui
{
VerticalSlider::VerticalSlider (const chowdsp::FloatParameter& p, const chowdsp::HostContextProvider& hcp)
    : param (p),
      hostContextProvider (hcp)
{
    if (hostContextProvider.supportsParameterModulation())
        startTimerHz (32);

    setSliderStyle (juce::Slider::LinearVertical);
    setColour (juce::Slider::backgroundColourId, colours::backgroundLight.brighter (0.1f));
    setColour (juce::Slider::textBoxTextColourId, colours::linesColour);
    setColour (juce::Slider::textBoxOutlineColourId, colours::linesColour.withAlpha (0.75f));
    setColour (juce::Slider::textBoxBackgroundColourId, colours::backgroundDark);
}

void VerticalSlider::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float modSliderPos)
{
    const auto horizontal = isHorizontal();
    auto trackWidth = juce::jmin (6.0f, horizontal ? (float) height * 0.25f : (float) width * 0.25f);

    juce::Point startPoint (horizontal ? (float) x : (float) x + (float) width * 0.5f,
                            horizontal ? (float) y + (float) height * 0.5f : (float) (height + y));

    juce::Point endPoint (horizontal ? (float) (width + x) : startPoint.x,
                          horizontal ? startPoint.y : (float) y);

    juce::Path backgroundTrack;
    backgroundTrack.startNewSubPath (startPoint);
    backgroundTrack.lineTo (endPoint);

    const auto alphaMult = isEnabled() ? 1.0f : 0.4f;
    g.setColour (findColour (juce::Slider::backgroundColourId).withAlpha (alphaMult));
    g.strokePath (backgroundTrack, { trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });

    juce::Path valueTrack;
    const auto maxPoint = [&]
    {
        auto kx = horizontal ? sliderPos : ((float) x + (float) width * 0.5f);
        auto ky = horizontal ? ((float) y + (float) height * 0.5f) : sliderPos;
        return juce::Point { kx, ky };
    }();
    const auto modPoint = [&]
    {
        auto kmx = horizontal ? modSliderPos : ((float) x + (float) width * 0.5f);
        auto kmy = horizontal ? ((float) y + (float) height * 0.5f) : modSliderPos;
        return juce::Point { kmx, kmy };
    }();

    valueTrack.startNewSubPath (startPoint);
    valueTrack.lineTo (modPoint);
    g.setColour (findColour (juce::Slider::thumbColourId).withAlpha (alphaMult));
    g.strokePath (valueTrack, { trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });

    auto thumbWidth = getLookAndFeel().getSliderThumbRadius (*this);
    auto thumbRect = juce::Rectangle<float> (static_cast<float> (thumbWidth),
                                             static_cast<float> (thumbWidth))
                         .withCentre (maxPoint);
    sharedAssets->knob->drawWithin (g, thumbRect, juce::RectanglePlacement::stretchToFit, alphaMult);
}

void VerticalSlider::paint (juce::Graphics& g)
{
    auto& lf = getLookAndFeel();
    auto layout = lf.getSliderLayout (*this);
    const auto sliderRect = layout.sliderBounds;

    modulatedValue = param.getCurrentValue();
    const auto normRange = juce::NormalisableRange { getRange() };
    const auto sliderPos = getPositionOfValue (getValue());
    const auto modSliderPos = getPositionOfValue (modulatedValue);
    drawLinearSlider (g,
                      sliderRect.getX(),
                      sliderRect.getY(),
                      sliderRect.getWidth(),
                      sliderRect.getHeight(),
                      sliderPos,
                      modSliderPos);
}

void VerticalSlider::mouseDown (const juce::MouseEvent& e)
{
    if (e.mods.isPopupMenu())
    {
        chowdsp::SharedLNFAllocator lnfAllocator;
        hostContextProvider.showParameterContextPopupMenu (param, {}, lnfAllocator->getLookAndFeel<lnf::MenuLNF>());
        return;
    }

    Slider::mouseDown (e);
}

void VerticalSlider::timerCallback()
{
    const auto newModulatedValue = param.getCurrentValue();
    if (std::abs (modulatedValue - newModulatedValue) < 0.01)
        return;

    repaint();
}
} // namespace gui
