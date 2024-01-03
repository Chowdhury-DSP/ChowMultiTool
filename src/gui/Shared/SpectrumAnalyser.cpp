#include "SpectrumAnalyser.h"
#include "gui/Shared/Colours.h"

SpectrumAnalyser::SpectrumAnalyser (const chowdsp::SpectrumPlotBase& eqPlot,
                                    gui::SpectrumAnalyserTask::PrePostPair spectrumAnalyserTasks)
    : eqPlot (eqPlot),
      preTask (spectrumAnalyserTasks.first.has_value() ? std::ref (spectrumAnalyserTasks.first).get() : std::nullopt),
      postTask (spectrumAnalyserTasks.second.has_value() ? std::ref (spectrumAnalyserTasks.second).get() : std::nullopt)
{
    minFrequencyHz.store (eqPlot.params.minFrequencyHz);
    maxFrequencyHz.store (eqPlot.params.maxFrequencyHz);
}

SpectrumAnalyser::~SpectrumAnalyser()
{
    if (preTask && preTask->get().isTaskRunning())
        preTask->get().setShouldBeRunning (false);
    if (postTask && postTask->get().isTaskRunning())
        postTask->get().setShouldBeRunning (false);
}

void SpectrumAnalyser::paint (juce::Graphics& g)
{
    const auto paintSpectrum = [&g, &plot = std::as_const (eqPlot)] (const DrawOptions& drawOptions, const juce::Path& path)
    {
        if (drawOptions.drawFill)
        {
            float gradientStart = plot.getYCoordinateForDecibels (-30.0f);
            auto gradientEnd = (float) plot.getHeight();

            juce::ColourGradient lowFreqGradient = juce::ColourGradient::vertical (
                drawOptions.gradientStartColour,
                gradientStart,
                drawOptions.gradientEndColour,
                gradientEnd);
            g.setGradientFill (lowFreqGradient);
            g.fillPath (path);
        }

        if (drawOptions.drawLine)
        {
            g.setColour (drawOptions.lineColour);
            g.strokePath (path, juce::PathStrokeType (1));
        }
    };

    if (showPreEQ)
        paintSpectrum (preEQDrawOptions, prePath);

    if (showPostEQ)
        paintSpectrum (postEQDrawOptions, postPath);
}

void SpectrumAnalyser::visibilityChanged()
{
    if (isVisible())
    {
        if (preTask)
        {
            preTask->get().reset();
            preTask->get().setShouldBeRunning (showPreEQ);
        }
        if (postTask)
        {
            postTask->get().reset();
            postTask->get().setShouldBeRunning (showPostEQ);
        }
        startTimerHz (32);
    }
    else
    {
        if (preTask)
            preTask->get().setShouldBeRunning (false);
        if (postTask)
            postTask->get().setShouldBeRunning (false);
        stopTimer();
    }
}

void SpectrumAnalyser::setShouldShowPreEQ (bool shouldShow)
{
    showPreEQ = shouldShow;
    preTask->get().setShouldBeRunning (showPreEQ && isVisible());
}

void SpectrumAnalyser::setShouldShowPostEQ (bool shouldShow)
{
    showPostEQ = shouldShow;
    postTask->get().setShouldBeRunning (showPostEQ && isVisible());
}

void SpectrumAnalyser::timerCallback()
{
    if (preTask && showPreEQ)
        updatePlotPath (prePath, preTask->get());
    if (postTask && showPostEQ)
        updatePlotPath (postPath, postTask->get());
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
        if (freqAxis[i] < minFrequencyHz.load() / 2.0f || freqAxis[i] > maxFrequencyHz.load() * 1.01f)
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
