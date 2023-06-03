#pragma once

#include <pch.h>

namespace gui
{
namespace detail
{
    constexpr float getFreqLineIncrement (float curFreq)
    {
        return (float) gcem::pow (10, (int) gcem::floor (gcem::log10 (curFreq)));
    }

    constexpr float getFirstFreqLine (float startFreq)
    {
        const auto increment = getFreqLineIncrement (startFreq);
        return increment * gcem::floor ((startFreq + increment - 1.0f) / increment);
    }
    static_assert (getFirstFreqLine (10.5f) == 10.0f);
    static_assert (getFirstFreqLine (11.0f) == 20.0f);
    static_assert (getFirstFreqLine (18.0f) == 20.0f);
    static_assert (getFirstFreqLine (20.0f) == 20.0f);
    static_assert (getFirstFreqLine (90.0f) == 90.0f);
    static_assert (getFirstFreqLine (95.0f) == 100.0f);

    constexpr float getLastFreqLine (float endFreq)
    {
        const auto increment = getFreqLineIncrement (endFreq);
        return increment * gcem::floor (endFreq / increment);
    }
    static_assert (getLastFreqLine (20'000.0f) == 20000.0f);
    static_assert (getLastFreqLine (22'000.0f) == 20000.0f);
    static_assert (getLastFreqLine (20'001.0f) == 20000.0f);
    static_assert (getLastFreqLine (19'999.0f) == 10000.0f);

    constexpr float getNextFreqLine (float prevFreqLine)
    {
        return prevFreqLine + getFreqLineIncrement (prevFreqLine);
    }

    constexpr size_t countFreqLines (float startFreqHz, float endFreqHz)
    {
        size_t count = 0;
        auto freq = getFirstFreqLine (startFreqHz);
        while (freq <= getLastFreqLine (endFreqHz))
        {
            freq = getNextFreqLine (freq);
            ++count;
        }
        return count;
    }
    static_assert (countFreqLines (20, 20000) == 28);
    static_assert (countFreqLines (18, 22000) == 28);

    template <int startFreqHz, int endFreqHz>
    constexpr auto getFreqLines()
    {
        constexpr auto numFreqLines = countFreqLines ((float) startFreqHz, (float) endFreqHz);
        std::array<float, numFreqLines> lines {};
        lines[0] = getFirstFreqLine ((float) startFreqHz);
        for (size_t count = 1; count < lines.size(); ++count)
            lines[count] = getNextFreqLine (lines[count - 1]);
        return lines;
    }
} // namespace detail

template <int startFreqHz, int endFreqHz>
void drawFrequencyLines (const chowdsp::SpectrumPlotBase& plotBase,
                         juce::Graphics& g,
                         const std::initializer_list<float>& majorLines = { 100.0f, 1'000.0f, 10'000.0f },
                         juce::Colour majorLinesColour = juce::Colours::white.withAlpha (0.5f),
                         juce::Colour minorLinesColour = juce::Colours::white.withAlpha (0.25f))
{
    g.setColour (minorLinesColour);
    plotBase.drawFrequencyLines (g, detail::getFreqLines<startFreqHz, endFreqHz>(), 1.0f);

    g.setColour (majorLinesColour);
    plotBase.drawFrequencyLines (g, majorLines, 1.0f);
}

inline void drawMagnitudeLines (const chowdsp::SpectrumPlotBase& plotBase,
                                juce::Graphics& g,
                                const std::initializer_list<float>& minorLines = { -30.0f, -20.0f, -10.0f, 10.0f, 20.0f, 30.0f },
                                const std::initializer_list<float>& majorLines = { 0.0f },
                                juce::Colour majorLinesColour = juce::Colours::white.withAlpha (0.5f),
                                juce::Colour minorLinesColour = juce::Colours::white.withAlpha (0.25f))
{
    g.setColour (minorLinesColour);
    plotBase.drawMagnitudeLines (g, minorLines);

    g.setColour (majorLinesColour);
    plotBase.drawMagnitudeLines (g, majorLines);
}

static constexpr auto labelMagnitudeDb = 18.0f;

inline void drawMagnitudeLabels (juce::Graphics& g, const chowdsp::SpectrumPlotBase& plot, float maxGain, int numDbValues)
{
    const int textWidth = 50;
    const int textHeight = plot.getHeight() / 25;
    const float step = maxGain / int (numDbValues / 2);
    int yPosition[numDbValues];
    int xPosition = plot.getX() + 3;

    for (int i = 0; i < numDbValues; i++)
    {
        float dBValue;
        if (i == numDbValues / 2)
            dBValue = 0.0f;
        else if (i > numDbValues / 2)
            dBValue = -step * float (i - numDbValues / 2);
        else
            dBValue = step * float (numDbValues / 2 - i);

        yPosition[i] = int (plot.getYCoordinateForDecibels (dBValue));
        juce::String text = juce::String (dBValue) + " dB";

        g.setFont (textHeight);
        g.setColour (juce::Colours::white.withAlpha (0.5f));
        g.drawFittedText (text, xPosition, yPosition[i], textWidth, textHeight, juce::Justification::left, 1);
    }
}

inline void drawFrequencyLabels (juce::Graphics& g, const chowdsp::SpectrumPlotBase& plot, float frequency, float maxGain)
{
    int xPosition = int (plot.getXCoordinateForFrequency (frequency));
    int yPosition = int (plot.getYCoordinateForDecibels (maxGain));

    juce::String unit = (frequency >= 1000) ? "KHz" : "Hz";
    juce::String text = juce::String (frequency / ((frequency >= 1000) ? 1000 : 1)) + " " + unit;

    g.setColour (juce::Colours::white.withAlpha (0.5f));
    g.drawFittedText (text, xPosition, yPosition, 100, 28, juce::Justification::topLeft, 1);
}
} // namespace gui
