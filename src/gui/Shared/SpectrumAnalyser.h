#pragma once
#include "dsp/MultiToolProcessor.h"
#include "dsp/Shared/EQHelpers.h"

class SpectrumAnalyser : public juce::Component,
                         public juce::Timer
{
public:
    SpectrumAnalyser(const chowdsp::SpectrumPlotBase& eqPlot, EQHelpers& helper);
    ~SpectrumAnalyser();
    void paint (juce::Graphics& g) override;
    void visibilityChanged() override;
    void timerCallback() override;
    void updatePlotPath();

private:
    juce::Path path;
    const chowdsp::SpectrumPlotBase& eqPlot;
    EQHelpers::SpectrumAnalyserBackgroundTask& task;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumAnalyser)


};

