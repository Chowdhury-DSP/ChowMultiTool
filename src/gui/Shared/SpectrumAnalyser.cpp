#include "SpectrumAnalyser.h"

SpectrumAnalyser::SpectrumAnalyser (const chowdsp::SpectrumPlotBase& eqPlot, EQHelpers& helper) : eqPlot (eqPlot),
                                                                                                  task (helper.SpectrumAnalyserTask)

{
}

SpectrumAnalyser::~SpectrumAnalyser()
{
    if (task.isTaskRunning())
        task.setShouldBeRunning (false);
}

void SpectrumAnalyser::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::teal.withAlpha (0.4f));
    g.fillPath (path);
    g.setColour (juce::Colours::orange);
    g.strokePath (path, juce::PathStrokeType (2.0f));
}

void SpectrumAnalyser::visibilityChanged()
{
    if (isVisible())
    {
        task.setShouldBeRunning (true);
        startTimerHz (32);
    }
    else
    {
        task.setShouldBeRunning (false);
        stopTimer();
    }
}

void SpectrumAnalyser::timerCallback()
{
    updatePlotPath();
}

void SpectrumAnalyser::updatePlotPath()
{
    path.clear();

    const auto& freqAxis = task.fftFreqs; //spectrum's frequency axis
    const auto& magResponseDBSmoothed = task.fftMagsSmoothedDB; //magnitude response

    bool started = false;
    const auto nPoints = freqAxis.size(); //number of points on the frequency axis
    for (size_t i = 0; i < nPoints; ++i) //loop over frequency points
    {
        if (freqAxis[i] < eqPlot.params.minFrequencyHz / 2.0f || freqAxis[i] > eqPlot.params.maxFrequencyHz * 1.01f)
            continue;

        auto xDraw = eqPlot.getXCoordinateForFrequency (freqAxis[i]);
        auto yDraw = eqPlot.getYCoordinateForDecibels (magResponseDBSmoothed[i]);

        if (! started)
        {
            path.startNewSubPath (xDraw, yDraw);
            started = true;
        }
        else
        {
            path.lineTo (xDraw, yDraw);
        }
    }

    path.lineTo (juce::Point { getWidth(), getHeight() }.toFloat());
    path.lineTo (juce::Point { 0, getHeight() }.toFloat());
    path.closeSubPath();

    repaint();
}