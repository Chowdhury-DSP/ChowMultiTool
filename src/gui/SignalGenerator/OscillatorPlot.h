#pragma once

#include <pch.h>

namespace chowdsp
{
class OscillatorPlot
{
public:
    OscillatorPlot() = default;

    void updateSize (int width, int height);

    void drawPlot (juce::Graphics& g, const juce::PathStrokeType& strokeType = juce::PathStrokeType { 1.0f });

    void updatePlotConfig (float sampleRate, float secondsToShow);

    void updatePlot (juce::Component* parent = nullptr);

    //    std::function<void()> plotUpdateCallback;
    std::function<void (const chowdsp::BufferView<float>&)> plotUpdateCallback;

private:
    float width = 0.0f;
    float height = 0.0f;

    juce::Path plotPath;
    chowdsp::Buffer<float> plotBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscillatorPlot)
};
} // namespace chowdsp