#include "SpectrumAnalyser.h"

SpectrumAnalyser::SpectrumAnalyser (const chowdsp::SpectrumPlotBase& eqPlot, EQHelpers& helper)
    : eqPlot (eqPlot),
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
    //    g.fillAll(juce::Colours::pink.withAlpha(0.4f));

    g.setGradientFill (juce::ColourGradient::vertical (juce::Colours::grey,
                                                       eqPlot.getYCoordinateForDecibels (0.0f),
                                                       juce::Colours::black,
                                                       (float) getHeight()));
    g.fillPath (path);
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

    const juce::ScopedLock sl { task.mutex };
    const auto& freqAxis = task.fftFreqs; //spectrum's frequency axis
    const auto& magResponseDBSmoothed = task.fftMagsSmoothedDB; //magnitude response

    bool started = false;
    const auto nPoints = freqAxis.size(); //number of points on the frequency axis
    for (size_t i = 0; i < nPoints;) //loop over frequency points
    {
        if (freqAxis[i] < eqPlot.params.minFrequencyHz / 2.0f || freqAxis[i] > eqPlot.params.maxFrequencyHz * 1.01f)
        {
            i++;
            continue;
        }

        if (! started)
        {
            auto xDraw = eqPlot.getXCoordinateForFrequency (freqAxis[i]);
            auto yDraw = eqPlot.getYCoordinateForDecibels (magResponseDBSmoothed[i]);
            path.startNewSubPath (xDraw, yDraw);
            started = true;
            i += 1;
        }
        else
        {
            if (i + 2 < nPoints)
            {
                auto xDraw1 = eqPlot.getXCoordinateForFrequency (freqAxis[i]);
                auto yDraw1 = eqPlot.getYCoordinateForDecibels (magResponseDBSmoothed[i]);
                auto xDraw2 = eqPlot.getXCoordinateForFrequency (freqAxis[i + 1]);
                auto yDraw2 = eqPlot.getYCoordinateForDecibels (magResponseDBSmoothed[i + 1]);
                auto xDraw3 = eqPlot.getXCoordinateForFrequency (freqAxis[i + 2]);
                auto yDraw3 = eqPlot.getYCoordinateForDecibels (magResponseDBSmoothed[i + 2]);
                path.cubicTo ({ xDraw1, yDraw1 }, { xDraw2, yDraw2 }, { xDraw3, yDraw3 });
            }
            i += 3;
        }
    }

    path.lineTo (juce::Point { getWidth(), getHeight() }.toFloat());
    path.lineTo (juce::Point { 0, getHeight() }.toFloat());
    path.closeSubPath();

    repaint();
}