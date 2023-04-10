#pragma once

#include "dsp/Waveshaper/SplineHelpers.h"

namespace gui::waveshaper::spline_view
{
using namespace dsp::waveshaper::spline;
constexpr auto dotDim = 0.035f;

inline float valueToXCoord (float value, float width)
{
    return juce::jmap (value, splineBounds.xMin, splineBounds.xMax, 0.0f, width);
}

inline float valueToYCoord (float value, float height)
{
    return juce::jmap (value, splineBounds.yMax, splineBounds.yMin, 0.0f, height);
}

inline float xCoordToValue (float xCoord, float width)
{
    return juce::jmap (xCoord, 0.0f, width, splineBounds.xMin, splineBounds.xMax);
}

inline float yCoordToValue (float yCoord, float height)
{
    return juce::jmap (yCoord, 0.0f, height, splineBounds.yMax, splineBounds.yMin);
}

inline juce::Point<float> pointToCoords (juce::Point<float> point, float width, float height)
{
    return { valueToXCoord (point.x, width), valueToYCoord (point.y, height) };
}

inline juce::Point<float> coordsToPoint (juce::Point<float> coords, float width, float height)
{
    return { xCoordToValue (coords.x, width), yCoordToValue (coords.y, height) };
}
} // namespace gui::waveshaper::spline_view
