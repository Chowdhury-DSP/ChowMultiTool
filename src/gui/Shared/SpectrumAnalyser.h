#pragma once
#include "SpectrumAnalyserTask.h"
#include "dsp/MultiToolProcessor.h"

class SpectrumAnalyser : public juce::Component,
                         public juce::Timer
{
public:
    SpectrumAnalyser (const chowdsp::SpectrumPlotBase& eqPlot, gui::SpectrumAnalyserTask& spectrumAnalyserTask);
    ~SpectrumAnalyser();
    void paint (juce::Graphics& g) override;
    void visibilityChanged() override;
    void timerCallback() override;
    void updatePlotPath(juce::Path& pathToUpdate);

private:
    juce::Path postPath;
//    juce::Path prePath;
    const chowdsp::SpectrumPlotBase& eqPlot;
    gui::SpectrumAnalyserTask::SpectrumAnalyserBackgroundTask& task;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumAnalyser)
};
