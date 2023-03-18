#include "WaveshaperDrawer.h"
#include "gui/Shared/Colours.h"

namespace gui::waveshaper
{
using namespace dsp::waveshaper;

constexpr auto dotDim = 0.035f;

float valueToXCoord (float value, float width)
{
    return juce::jmap (value, splineBounds.xMin, splineBounds.xMax, 0.0f, width);
}

float valueToYCoord (float value, float height)
{
    return juce::jmap (value, splineBounds.yMax, splineBounds.yMin, 0.0f, height);
}

float xCoordToValue (float xCoord, float width)
{
    return juce::jmap (xCoord, 0.0f, width, splineBounds.xMin, splineBounds.xMax);
}

float yCoordToValue (float yCoord, float height)
{
    return juce::jmap (yCoord, 0.0f, height, splineBounds.yMax, splineBounds.yMin);
}

juce::Point<float> pointToCoords (juce::Point<float> point, float width, float height)
{
    return { valueToXCoord (point.x, width), valueToYCoord (point.y, height) };
}

juce::Point<float> coordsToPoint (juce::Point<float> coords, float width, float height)
{
    return { xCoordToValue (coords.x, width), yCoordToValue (coords.y, height) };
}

WaveshaperDrawer::WaveshaperDrawer (dsp::waveshaper::ExtraState& extraState)
    : splineState (extraState.splineState)
{
}

chowdsp::WaveshaperPlotParams WaveshaperDrawer::getPlotParams()
{
    return dsp::waveshaper::splineBounds;
}

void WaveshaperDrawer::paint (juce::Graphics& g)
{
    const auto drawCircle = [&g, dim = dotDim * (float) getHeight()] (juce::Point<float> pos)
    {
        g.fillEllipse (juce::Rectangle { dim, dim }.withCentre (pos));
    };

    if (mousePos.has_value())
    {
        g.setColour (colours::plotColour.withAlpha (0.75f));
        drawCircle (*mousePos);
    }

    const auto floatWidth = (float) getWidth();
    const auto floatHeight = (float) getHeight();

    g.setColour (colours::plotColour);
    for (auto [idx, point] : chowdsp::enumerate (points))
    {
        drawCircle (pointToCoords (point, floatWidth, floatHeight));
    }

    if (! splineState.get().empty())
    {
        const auto splinePath = getDrawnPath();
        g.strokePath (splinePath, juce::PathStrokeType { juce::PathStrokeType::curved });
    }
}

juce::Path WaveshaperDrawer::getDrawnPath (std::optional<chowdsp::WaveshaperPlotParams>&& params) const
{
    const auto& splineInfo = splineState.get();
    if (splineInfo.empty())
        return {};

    if (! params.has_value())
        params.emplace (getPlotParams());

    const auto floatWidth = (float) getWidth();
    const auto floatHeight = (float) getHeight();

    juce::Path splinePath;
    splinePath.startNewSubPath ({ splineBounds.xMin, (float) evaluateSpline (splineInfo, (double) splineBounds.xMin) });
    const auto xInc = double ((splineBounds.xMax - splineBounds.xMin) / floatWidth);
    for (double x = (double) splineBounds.xMin + xInc; x <= (double) splineBounds.xMax; x += xInc)
    {
        splinePath.lineTo (juce::Point { x, evaluateSpline (splineInfo, x) }.toFloat());
    }

    splinePath.applyTransform (juce::AffineTransform::fromTargetPoints (juce::Point { params->xMin, params->yMin },
                                                                        juce::Point { 0.0f, floatHeight },
                                                                        juce::Point { params->xMin, params->yMax },
                                                                        juce::Point { 0.0f, 0.0f },
                                                                        juce::Point { params->xMax, params->yMax },
                                                                        juce::Point { floatWidth, 0.0f }));
    return splinePath;
}

void WaveshaperDrawer::visibilityChanged()
{
    if (isVisible())
    {
        points.clear();
        splineState.set ({});
    }
}

void WaveshaperDrawer::mouseDown (const juce::MouseEvent& e)
{
    mousePos.reset();

    const auto pos = e.getEventRelativeTo (this).getPosition().toFloat();
    if (points.empty() || pos.x > valueToXCoord (points.back().x, (float) getWidth()))
    {
        points.emplace_back (coordsToPoint (pos, (float) getWidth(), (float) getHeight()));
        splineState.set (createSpline (points));
    }
    repaint();
}

void WaveshaperDrawer::mouseMove (const juce::MouseEvent& e)
{
    const auto pos = e.getEventRelativeTo (this).getPosition().toFloat();
    if (points.empty() || pos.x > valueToXCoord (points.back().x, (float) getWidth()))
        mousePos.emplace (pos);
    else
        mousePos.reset();

    repaint();
}

void WaveshaperDrawer::mouseExit (const juce::MouseEvent&)
{
    mousePos.reset();
    repaint();
}
} // namespace gui::waveshaper
