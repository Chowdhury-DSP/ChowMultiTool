#pragma once
#include "SpectrumAnalyserTask.h"
#include "dsp/MultiToolProcessor.h"

class SpectrumAnalyser : public juce::Component,
                         public juce::Timer
{
public:
    enum class Type {
        Low,
        Mid,
        High,
        LowMid,
        HighMid,
        FullSpectrum
    };

    SpectrumAnalyser (const chowdsp::SpectrumPlotBase& eqPlot,
                      std::pair<gui::SpectrumAnalyserTask::Optional, gui::SpectrumAnalyserTask::Optional> spectrumAnalyserTasks,
                      Type type);
    ~SpectrumAnalyser() override;
    void paint (juce::Graphics& g) override;
    void visibilityChanged() override;
    void timerCallback() override;
    void updatePlotPath (juce::Path& pathToUpdate, gui::SpectrumAnalyserTask::SpectrumAnalyserBackgroundTask& taskToUpdate);
    void setShouldShowPreEQ (bool shouldShow);
    void setShouldShowPostEQ (bool shouldShow);
    void setGradientStartColour (juce::Colour gradientStart) { drawOptions.gradientStartColour = gradientStart; };
    void setGradientEndColour (juce::Colour gradientEnd) { drawOptions.gradientEndColour = gradientEnd; };
    void setLineColour (juce::Colour lineColour) { drawOptions.lineColour = lineColour; };
    Type getType() { return analyserType; }

    struct SpectrumDrawOptions
    {
        bool is_filled = false;
        juce::Colour gradientStartColour = juce::Colour (0xff008080).withAlpha (0.4f);
        juce::Colour gradientEndColour = juce::Colour (0xff00008b).withAlpha (0.4f);
        juce::Colour lineColour = juce::Colour (0xff008080).brighter();
    };

    std::atomic<float> minFrequencyHz;
    std::atomic<float> maxFrequencyHz;

private:

    using optionalBackgroundTask = std::optional<std::reference_wrapper<gui::SpectrumAnalyserTask::SpectrumAnalyserBackgroundTask>>;
    juce::Path prePath;
    juce::Path postPath;
    const chowdsp::SpectrumPlotBase& eqPlot;
    optionalBackgroundTask preTask;
    optionalBackgroundTask postTask;
    bool showPreEQ = true;
    bool showPostEQ = true;
    SpectrumDrawOptions drawOptions;
    Type analyserType;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumAnalyser)
};
