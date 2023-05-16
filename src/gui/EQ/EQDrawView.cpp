#include "EQDrawView.h"
#include "gui/Shared/Colours.h"

namespace gui::eq
{
float getMagnitudeAtFrequency (const EQPath& eqPath, float frequencyHz, const chowdsp::SpectrumPlotParams& plotParams)
{
    const auto xNorm = std::log (frequencyHz / plotParams.minFrequencyHz) / std::log (plotParams.maxFrequencyHz / plotParams.minFrequencyHz);
    const auto indexForFrequency = (size_t) juce::roundToInt (juce::jmap (xNorm, 0.0f, (float) maxNumDrawPoints - 1.0f));
    return eqPath[indexForFrequency].y;
}

EQDrawView::EQDrawView (const chowdsp::SpectrumPlotBase& plotBase)
    : spectrumPlot (plotBase)
{
}

void EQDrawView::paint (juce::Graphics& g)
{
    const auto drawCircle = [&g, dim = 0.035f * (float) getHeight()] (juce::Point<float> pos)
    {
        g.fillEllipse (juce::Rectangle { dim, dim }.withCentre (pos));
    };

    if (mousePos.has_value())
    {
        g.setColour (colours::thumbColours[0].withAlpha (0.75f));
        drawCircle (*mousePos);
    }

    const auto getPoint = [this] (float xCoord)
    {
        const auto freqHz = spectrumPlot.getFrequencyForXCoordinate (xCoord);
        const auto magDB = getMagnitudeAtFrequency (eqPath, freqHz, spectrumPlot.params);
        return juce::Point { xCoord, spectrumPlot.getYCoordinateForDecibels (magDB) };
    };

    juce::Path eqPlotPath;
    eqPlotPath.startNewSubPath (getPoint (0.0f));

    for (float x = 1.0f; x < (float) getWidth(); x += 0.5f)
    {
        eqPlotPath.lineTo (getPoint (x));
    }

    g.setColour (colours::thumbColours[0]);
    g.strokePath (eqPlotPath, juce::PathStrokeType { juce::PathStrokeType::curved });

    if (isOptimising)
    {
        g.setColour (juce::Colours::black.withAlpha (0.25f));
        g.fillRect (getParentComponent()->getLocalBounds());
    }
}

std::array<float, dsp::eq::EQOptimiser::numPoints> EQDrawView::getDrawnMagnitudeResponse()
{
    std::array<float, dsp::eq::EQOptimiser::numPoints> freqs = optimiser.freqs;
    std::array<float, dsp::eq::EQOptimiser::numPoints> magnitudeResponse {};
    for (size_t i = 0; i < dsp::eq::EQOptimiser::numPoints; i++)
    {
        magnitudeResponse[i] = gui::eq::getMagnitudeAtFrequency (eqPath, freqs[i], spectrumPlot.params);
    }
    return magnitudeResponse;
}

void EQDrawView::triggerOptimiser (chowdsp::EQ::StandardEQParameters<dsp::eq::EQToolParams::numBands>& eqParameters)
{
    isOptimising = true;
    repaint();
    juce::Thread::launch (
        [&]
        {
            auto desiredResponse = getDrawnMagnitudeResponse();
            optimiser.runOptimiser (std::move (desiredResponse));
            optimiser.updateEQParameters (eqParameters); // TODO: make this undo-able
            juce::MessageManager::callAsync ([this]
                                             {
                                                 onCompletedOptimisation();
                                                 isOptimising = false; });
        });
}

const dsp::eq::EQOptimiser& EQDrawView::getOptimiser() const
{
    return this->optimiser;
}

void EQDrawView::setEQPathPoint (juce::Point<float> point)
{
    const auto pointIndex = (size_t) juce::roundToInt (juce::jmap (point.x, 0.0f, (float) getWidth(), 0.0f, (float) maxNumDrawPoints - 1.0f));
    eqPath[pointIndex].y = juce::jmap ((float) getHeight() - point.y, 0.0f, (float) getHeight(), -18.0f, 18.0f);
}

void EQDrawView::mouseDown (const juce::MouseEvent& e)
{
    if (isOptimising)
        return;

    lastMouseDragPoint = e.getEventRelativeTo (this).getPosition().toFloat();
    setEQPathPoint (lastMouseDragPoint);
    repaint();
}

void EQDrawView::mouseDrag (const juce::MouseEvent& e)
{
    if (isOptimising)
        return;

    auto newMouseDragPoint = e.getEventRelativeTo (this).getPosition().toFloat();
    newMouseDragPoint.x = juce::jlimit (0.0f, (float) getWidth(), newMouseDragPoint.x);
    newMouseDragPoint.y = juce::jlimit (0.0f, (float) getHeight(), newMouseDragPoint.y);
    if (lastMouseDragPoint.x < newMouseDragPoint.x)
    {
        for (float x = lastMouseDragPoint.x; x <= newMouseDragPoint.x; x += 0.1f)
        {
            float pct = (x - lastMouseDragPoint.x) / (newMouseDragPoint.x - lastMouseDragPoint.x);
            setEQPathPoint ({ x, juce::jmap (pct, lastMouseDragPoint.y, newMouseDragPoint.y) });
        }
    }
    else if (newMouseDragPoint.x < lastMouseDragPoint.x)
    {
        for (float x = lastMouseDragPoint.x; x >= newMouseDragPoint.x; x -= 0.1f)
        {
            float pct = (x - lastMouseDragPoint.x) / (newMouseDragPoint.x - lastMouseDragPoint.x);
            setEQPathPoint ({ x, juce::jmap (pct, lastMouseDragPoint.y, newMouseDragPoint.y) });
        }
    }

    lastMouseDragPoint = newMouseDragPoint;
    mousePos.emplace (lastMouseDragPoint);

    repaint();
}

void EQDrawView::mouseMove (const juce::MouseEvent& e)
{
    const auto pos = e.getEventRelativeTo (this).getPosition().toFloat();
    mousePos.emplace (pos);
    repaint();
}

void EQDrawView::mouseExit (const juce::MouseEvent&)
{
    mousePos.reset();
    repaint();
}
} // namespace gui::eq
