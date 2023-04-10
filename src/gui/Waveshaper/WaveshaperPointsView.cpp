#include "WaveshaperPointsView.h"
#include "SplineViewHelpers.h"
#include "gui/Shared/Colours.h"

namespace gui::waveshaper
{
using namespace spline;

WaveshaperPointsView::WaveshaperPointsView (dsp::waveshaper::ExtraState& extraState, juce::UndoManager& undoManager)
    : splineState (extraState.pointsState),
      um (undoManager)
{
    points = DefaultVectorSplineCreator::call();
}

void WaveshaperPointsView::paint (juce::Graphics& g)
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

    const auto floatWidth = (float) getWidth();
    const auto floatHeight = (float) getHeight();

    g.setColour (colours::plotColour);
    for (auto [idx, point] : chowdsp::enumerate (points))
    {
        drawCircle (spline_view::pointToCoords (point, floatWidth, floatHeight));
    }

    if (! splineState.get().empty())
    {
        const auto splinePath = getDrawnPath();
        g.strokePath (splinePath, juce::PathStrokeType { juce::PathStrokeType::curved });
    }
}

juce::Path WaveshaperPointsView::getDrawnPath (std::optional<chowdsp::WaveshaperPlotParams>&& params) const
{
    const auto& splineInfo = createSpline (splineState.get());
    if (splineInfo.empty())
        return {};

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

void WaveshaperPointsView::visibilityChanged()
{
    if (isVisible())
    {
        prevPoints = points;
        points = {};
        splineState.set (points);
    }
    else
    {
        um.beginNewTransaction ("Waveshaper Spline Points");
        um.perform (new UndoableVectorSplineSet { splineState, prevPoints, getParentComponent() });
    }
}

void WaveshaperPointsView::mouseDown (const juce::MouseEvent& e)
{
    mousePos.reset();

    const auto pos = e.getEventRelativeTo (this).getPosition().toFloat();
    if (points.empty() || pos.x > spline_view::valueToXCoord (points.back().x, (float) getWidth()))
    {
        points.emplace_back (spline_view::coordsToPoint (pos, (float) getWidth(), (float) getHeight()));
        splineState.set (points);
    }
    repaint();
}

void WaveshaperPointsView::mouseMove (const juce::MouseEvent& e)
{
    const auto pos = e.getEventRelativeTo (this).getPosition().toFloat();
    if (points.empty() || pos.x > spline_view::valueToXCoord (points.back().x, (float) getWidth()))
        mousePos.emplace (pos);
    else
        mousePos.reset();

    repaint();
}

void WaveshaperPointsView::mouseExit (const juce::MouseEvent&)
{
    mousePos.reset();
    repaint();
}
}
