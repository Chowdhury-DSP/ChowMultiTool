#pragma once
#include "SpectrumAnalyserTask.h"
#include "dsp/MultiToolProcessor.h"

class SpectrumAnalyser : public juce::Component,
                         public juce::Timer
{
public:
    SpectrumAnalyser (const chowdsp::SpectrumPlotBase& eqPlot, std::pair<gui::SpectrumAnalyserTask::Optional, gui::SpectrumAnalyserTask::Optional> spectrumAnalyserTasks);
    ~SpectrumAnalyser() override;
    void paint (juce::Graphics& g) override;
    void visibilityChanged() override;
    void timerCallback() override;
    void updatePlotPath (juce::Path& pathToUpdate, gui::SpectrumAnalyserTask::SpectrumAnalyserBackgroundTask& taskToUpdate);
    void setShouldShowPreEQ (bool shouldShow);
    void setShouldShowPostEQ (bool shouldShow);

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
