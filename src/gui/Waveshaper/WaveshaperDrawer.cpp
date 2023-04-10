#include "WaveshaperDrawer.h"
#include "SplineViewHelpers.h"
#include "gui/Shared/Colours.h"

namespace gui::waveshaper
{
using namespace spline;

WaveshaperDrawer::WaveshaperDrawer (dsp::waveshaper::ExtraState& extraState, juce::UndoManager& undoManager)
    : freeDrawState (extraState.freeDrawState),
      um (undoManager)
{
    points = DefaultSplineCreator::call();
}

void WaveshaperDrawer::paint (juce::Graphics& g)
{
    const auto drawCircle = [&g, dim = spline_view::dotDim * (float) getHeight()] (juce::Point<float> pos)
    {
        g.fillEllipse (juce::Rectangle { dim, dim }.withCentre (pos));
    };

    if (mousePos.has_value())
    {
        g.setColour (colours::plotColour.withAlpha (0.75f));
        drawCircle (*mousePos);
    }

    g.setColour (colours::plotColour);
    const auto splinePath = getDrawnPath();
    g.strokePath (splinePath, juce::PathStrokeType { juce::PathStrokeType::curved });
}

juce::Path WaveshaperDrawer::getDrawnPath (std::optional<chowdsp::WaveshaperPlotParams>&& params) const
{
    const auto splineInfo = createSpline (freeDrawState.get());

    if (! params.has_value())
        params.emplace (splineBounds);

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
        prevPoints = points;
        points = DefaultSplineCreator::call();
        freeDrawState.set (points);
    }
    else
    {
        um.beginNewTransaction ("Waveshaper Free-Draw");
        um.perform (new UndoableSplineSet { freeDrawState, prevPoints, getParentComponent() });
    }
}

void WaveshaperDrawer::setSplinePoint (juce::Point<float> point)
{
    static constexpr auto scalerAndOffset = getSplineScalerAndOffset<float>();
    static constexpr auto scaler = scalerAndOffset.first;
    static constexpr auto offset = scalerAndOffset.second;

    const auto pointIndex = std::min ((size_t) juce::truncatePositiveToUnsignedInt (spline_view::xCoordToValue (point.x, (float) getWidth()) * scaler + offset),
                                      (size_t) maxNumDrawPoints - 1);
    points[pointIndex].y = spline_view::yCoordToValue (point.y, (float) getHeight());
    freeDrawState.set (points);
}

void WaveshaperDrawer::mouseDown (const juce::MouseEvent& e)
{
    lastMouseDragPoint = e.getEventRelativeTo (this).getPosition().toFloat();
    setSplinePoint (lastMouseDragPoint);
    repaint();
}

void WaveshaperDrawer::mouseDrag (const juce::MouseEvent& e)
{
    auto newMouseDragPoint = e.getEventRelativeTo (this).getPosition().toFloat();
    newMouseDragPoint.x = juce::jlimit (0.0f, (float) getWidth(), newMouseDragPoint.x);
    newMouseDragPoint.y = juce::jlimit (0.0f, (float) getHeight(), newMouseDragPoint.y);
    if (lastMouseDragPoint.x < newMouseDragPoint.x)
    {
        for (float x = lastMouseDragPoint.x; x <= newMouseDragPoint.x; x += 0.1f)
        {
            float pct = (x - lastMouseDragPoint.x) / (newMouseDragPoint.x - lastMouseDragPoint.x);
            setSplinePoint ({ x, juce::jmap (pct, lastMouseDragPoint.y, newMouseDragPoint.y) });
        }
    }
    else if (newMouseDragPoint.x < lastMouseDragPoint.x)
    {
        for (float x = lastMouseDragPoint.x; x >= newMouseDragPoint.x; x -= 0.1f)
        {
            float pct = (x - lastMouseDragPoint.x) / (newMouseDragPoint.x - lastMouseDragPoint.x);
            setSplinePoint ({ x, juce::jmap (pct, lastMouseDragPoint.y, newMouseDragPoint.y) });
        }
    }

    lastMouseDragPoint = newMouseDragPoint;
    mousePos.emplace (lastMouseDragPoint);

    repaint();
}

void WaveshaperDrawer::mouseMove (const juce::MouseEvent& e)
{
    const auto pos = e.getEventRelativeTo (this).getPosition().toFloat();
    mousePos.emplace (pos);

    repaint();
}

void WaveshaperDrawer::mouseExit (const juce::MouseEvent&)
{
    mousePos.reset();
    repaint();
}
} // namespace gui::waveshaper
