#include "SpectrumAnalyser.h"
#include "gui/Shared/Colours.h"

SpectrumAnalyser::SpectrumAnalyser (const chowdsp::SpectrumPlotBase& eqPlot, gui::SpectrumAnalyserTask& spectrumAnalyserTask)
    : eqPlot (eqPlot),
      task (spectrumAnalyserTask.SpectrumAnalyserUITask)

{
}

SpectrumAnalyser::~SpectrumAnalyser()
{
    if (task.isTaskRunning())
        task.setShouldBeRunning (false);
}

void SpectrumAnalyser::paint (juce::Graphics& g)
{
//    g.fillAll(juce::Colours::wheat.withAlpha(0.4f));
//    g.setColour(gui::logo::colours::backgroundBlue.brighter(0.4f));
//    g.strokePath(prePath, juce::PathStrokeType(1));
    g.setGradientFill (juce::ColourGradient::vertical (gui::logo::colours::backgroundBlue.withAlpha(0.4f),
                                                       eqPlot.getYCoordinateForDecibels (0.0f),
                                                       gui::logo::colours::backgroundBlue.darker().withAlpha(0.4f),
                                                       (float) getHeight()));
    g.fillPath (postPath);
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
    updatePlotPath(postPath);
//    updatePlotPath(prePath);
}

void SpectrumAnalyser::updatePlotPath(juce::Path& pathToUpdate)
{
    pathToUpdate.clear();

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