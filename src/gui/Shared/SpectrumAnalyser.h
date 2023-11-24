#pragma once
#include "SpectrumAnalyserTask.h"
#include "dsp/MultiToolProcessor.h"

class SpectrumAnalyser : public juce::Component,
                         public juce::Timer
{
public:
    SpectrumAnalyser (const chowdsp::SpectrumPlotBase& eqPlot, std::pair<gui::SpectrumAnalyserTask&, gui::SpectrumAnalyserTask&> spectrumAnalyserTasks);
    ~SpectrumAnalyser();
    void paint (juce::Graphics& g) override;
    void visibilityChanged() override;
    void timerCallback() override;
    void updatePlotPath (juce::Path& pathToUpdate, gui::SpectrumAnalyserTask::SpectrumAnalyserBackgroundTask& taskToUpdate);

private:
    juce::Path prePath;
    juce::Path postPath;
    const chowdsp::SpectrumPlotBase& eqPlot;
    gui::SpectrumAnalyserTask::SpectrumAnalyserBackgroundTask& preTask;
    gui::SpectrumAnalyserTask::SpectrumAnalyserBackgroundTask& postTask;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumAnalyser)
};
