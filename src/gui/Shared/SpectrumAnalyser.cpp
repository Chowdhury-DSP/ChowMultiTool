#include "SpectrumAnalyser.h"
#include "gui/Shared/Colours.h"

SpectrumAnalyser::SpectrumAnalyser (const chowdsp::SpectrumPlotBase& eqPlot, std::pair<gui::SpectrumAnalyserTask&, gui::SpectrumAnalyserTask&> spectrumAnalyserTasks)
    : eqPlot (eqPlot),
      preTask (spectrumAnalyserTasks.first.SpectrumAnalyserUITask),
      postTask (spectrumAnalyserTasks.second.SpectrumAnalyserUITask)

{
}

SpectrumAnalyser::~SpectrumAnalyser()
{
    if (preTask.isTaskRunning())
        preTask.setShouldBeRunning (false);
    if (postTask.isTaskRunning())
        postTask.setShouldBeRunning (false);
}

void SpectrumAnalyser::paint (juce::Graphics& g)
{
    //    g.fillAll(juce::Colours::whitesmoke.withAlpha(0.4f));

    if (showPreEQ)
    {
        g.setColour (gui::logo::colours::backgroundBlue.brighter (0.4f));
        g.strokePath (prePath, juce::PathStrokeType (1));
    }

    if (showPostEQ)
    {
        g.setGradientFill (juce::ColourGradient::vertical (gui::logo::colours::backgroundBlue.withAlpha (0.4f),
                                                           eqPlot.getYCoordinateForDecibels (0.0f),
                                                           gui::logo::colours::backgroundBlue.darker().withAlpha (0.4f),
                                                           (float) getHeight()));
        g.fillPath (postPath);
    }
}

void SpectrumAnalyser::visibilityChanged()
{
    if (isVisible())
    {
        preTask.setShouldBeRunning (true);
        postTask.setShouldBeRunning (true);
        startTimerHz (32);
    }
    else
    {
        preTask.setShouldBeRunning (true);
        postTask.setShouldBeRunning (false);
        stopTimer();
    }
}

void SpectrumAnalyser::timerCallback()
{
    if (showPreEQ)
        updatePlotPath (prePath, preTask);
    if (showPostEQ)
        updatePlotPath (postPath, postTask);
}

void SpectrumAnalyser::updatePlotPath (juce::Path& pathToUpdate, gui::SpectrumAnalyserTask::SpectrumAnalyserBackgroundTask& taskToUpdate)
{
    pathToUpdate.clear();

    const juce::ScopedLock sl { taskToUpdate.mutex };
    const auto& freqAxis = taskToUpdate.fftFreqs;
    const auto& magResponseDBSmoothed = taskToUpdate.fftMagsSmoothedDB;

    bool started = false;
    const auto nPoints = freqAxis.size();
    for (size_t i = 0; i < nPoints;)
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
            pathToUpdate.startNewSubPath (xDraw, yDraw);
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
                pathToUpdate.cubicTo ({ xDraw1, yDraw1 }, { xDraw2, yDraw2 }, { xDraw3, yDraw3 });
            }
            i += 3;
        }
    }

    pathToUpdate.lineTo (juce::Point { getWidth(), getHeight() }.toFloat());
    pathToUpdate.lineTo (juce::Point { 0, getHeight() }.toFloat());
    pathToUpdate.closeSubPath();

    repaint();
}