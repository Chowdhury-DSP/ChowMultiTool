#pragma once
#include "SpectrumAnalyserTask.h"
#include "dsp/MultiToolProcessor.h"

class SpectrumAnalyser : public juce::Component,
                         public juce::Timer
{
public:
    SpectrumAnalyser (const chowdsp::SpectrumPlotBase& eqPlot,
                      gui::SpectrumAnalyserTask::PrePostPair spectrumAnalyserTasks);
    ~SpectrumAnalyser() override;
    void paint (juce::Graphics& g) override;
    void visibilityChanged() override;
    void timerCallback() override;
    void updatePlotPath (juce::Path& pathToUpdate, gui::SpectrumAnalyserTask::SpectrumAnalyserBackgroundTask& taskToUpdate);
    void setShouldShowPreEQ (bool shouldShow);
    void setShouldShowPostEQ (bool shouldShow);

    struct DrawOptions
    {
        bool drawFill = false;
        bool drawLine = false;
        juce::Colour gradientStartColour = juce::Colour (0xff008080).withAlpha (0.4f);
        juce::Colour gradientEndColour = juce::Colour (0xff00008b).withAlpha (0.4f);
        juce::Colour lineColour = juce::Colour (0xff008080).brighter();
    };

    DrawOptions preEQDrawOptions { .drawLine = true };
    DrawOptions postEQDrawOptions { .drawFill = true };

    std::atomic<float> minFrequencyHz;
    std::atomic<float> maxFrequencyHz;

private:
    using OptionalBackgroundTask = gui::SpectrumAnalyserTask::OptionalBackgroundTask;
    juce::Path prePath;
    juce::Path postPath;
    const chowdsp::SpectrumPlotBase& eqPlot;
    OptionalBackgroundTask preTask;
    OptionalBackgroundTask postTask;
    bool showPreEQ = true;
    bool showPostEQ = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumAnalyser)
};
