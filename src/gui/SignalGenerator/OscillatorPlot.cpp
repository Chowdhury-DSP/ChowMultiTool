#include "OscillatorPlot.h"

namespace chowdsp
{
void OscillatorPlot::updateSize (int newWidth, int newHeight)
{
    width = (float) newWidth;
    height = (float) newHeight;

    updatePlot();
}

void OscillatorPlot::drawPlot (juce::Graphics& g, const juce::PathStrokeType& strokeType)
{
    if (width <= 0.0f || height <= 0.0f)
        return;

    g.strokePath (plotPath, strokeType);
}

void OscillatorPlot::updatePlotConfig (float sampleRate, float secondsToShow)
{
    const auto samplesToShow = juce::roundToInt (sampleRate * secondsToShow);
    plotBuffer.setMaxSize (1, samplesToShow);
    plotBuffer.clear();
}

template <bool isStart = false>
void plotSample (juce::Path& path, float xCoord, float yCoord)
{
    if constexpr (isStart)
        path.startNewSubPath (juce::Point { xCoord, yCoord });
    else
        path.lineTo (juce::Point { xCoord, yCoord });
}

void OscillatorPlot::updatePlot (juce::Component* parent)
{
    if (width <= 0.0f || height <= 0.0f)
        return;

    const auto samplesToShow = plotBuffer.getNumSamples();
    plotPath.clear();
    if (samplesToShow == 0 || plotUpdateCallback == nullptr)
    {
        plotPath.startNewSubPath (juce::Point { 0.0f, height * 0.5f });
        plotPath.lineTo (juce::Point { width, height * 0.5f });
        return;
    }

    plotUpdateCallback (plotBuffer);

    plotPath.preallocateSpace (3 * samplesToShow);
    auto* plotBufferData = plotBuffer.getReadPointer (0);
    plotSample<true> (plotPath, 0.0f, plotBufferData[0]);
    for (int n = 1; n < samplesToShow; ++n)
        plotSample (plotPath, (float) n, plotBufferData[n]);

    plotPath.applyTransform (juce::AffineTransform::scale ((float) width / (float) samplesToShow,
                                                           (float) height * 0.5f)
                                 .translated (0.0f, (float) height * 0.5f));

    if (parent != nullptr)
        parent->repaint();
}
} // namespace chowdsp
