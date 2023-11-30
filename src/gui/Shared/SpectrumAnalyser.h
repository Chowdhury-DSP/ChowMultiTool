#pragma once
#include "SpectrumAnalyserTask.h"
#include "dsp/MultiToolProcessor.h"

using optionalSpectrumBackgroundTask = std::optional<std::reference_wrapper<gui::SpectrumAnalyserTask::SpectrumAnalyserBackgroundTask>>;

class SpectrumAnalyser : public juce::Component,
                         public juce::Timer
{
public:
    SpectrumAnalyser (const chowdsp::SpectrumPlotBase& eqPlot, std::pair<optionalSpectrumBackgroundTask, optionalSpectrumBackgroundTask> spectrumAnalyserTasks);
    ~SpectrumAnalyser() override;
    void paint (juce::Graphics& g) override;
    void visibilityChanged() override;
    void timerCallback() override;
    void updatePlotPath (juce::Path& pathToUpdate, gui::SpectrumAnalyserTask::SpectrumAnalyserBackgroundTask& taskToUpdate);
    bool getShowPreEQ() { return showPreEQ; }
    bool getShowPostEQ() { return showPostEQ; }
    void setShowPreEQ (bool showPreSpectrum) { showPreEQ = showPreSpectrum; }
    void setShowPostEQ (bool showPostSpectrum) { showPostEQ = showPostSpectrum; }

private:
    using optionalBackgroundTask = std::optional<std::reference_wrapper<gui::SpectrumAnalyserTask::SpectrumAnalyserBackgroundTask>>;
    juce::Path prePath;
    juce::Path postPath;
    const chowdsp::SpectrumPlotBase& eqPlot;
    optionalBackgroundTask preTask;
    optionalBackgroundTask postTask;
    bool showPreEQ = true;
    bool showPostEQ = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumAnalyser)
};
