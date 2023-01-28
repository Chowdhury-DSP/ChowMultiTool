#include "BandSplitterPlot.h"

namespace
{
constexpr int numBands = 2;
}

BandSplitterPlot::InternalSlider::InternalSlider (chowdsp::FloatParameter& cutoff,
                                                  chowdsp::EQ::EqualizerPlot& plot)
    : cutoffParam (cutoff), plotBase (plot)
{
    setTextBoxStyle (NoTextBox, false, 0, 0);
    setSliderStyle (LinearHorizontal);
    setMouseCursor (juce::MouseCursor::StandardCursorType::LeftRightResizeCursor);
}

void BandSplitterPlot::InternalSlider::paint (juce::Graphics& g)
{
    g.setColour (juce::Colours::gold.withAlpha (0.5f));
    g.fillRect (getThumbBounds());
}

bool BandSplitterPlot::InternalSlider::hitTest (int x, int y)
{
    return getThumbBounds().contains (x, y);
}

double BandSplitterPlot::InternalSlider::proportionOfLengthToValue (double proportion)
{
    return (double) plotBase.getFrequencyForXCoordinate ((float) proportion * (float) getWidth());
}

double BandSplitterPlot::InternalSlider::valueToProportionOfLength (double value)
{
    return (double) plotBase.getXCoordinateForFrequency ((float) value) / (double) getWidth();
}

juce::Rectangle<int> BandSplitterPlot::InternalSlider::getThumbBounds() const
{
    static constexpr auto lowMult = 0.9f;
    static constexpr auto highMult = 1.0f / lowMult;
    const auto cutoffBarXStart = juce::roundToInt (plotBase.getXCoordinateForFrequency (cutoffParam.get() * lowMult));
    const auto cutoffBarXEnd = juce::roundToInt (plotBase.getXCoordinateForFrequency (cutoffParam.get() * highMult));
    return juce::Rectangle<int> {}
        .withPosition (cutoffBarXStart, 0)
        .withRight (cutoffBarXEnd)
        .withHeight (getHeight());
}

BandSplitterPlot::BandSplitterPlot (State& pluginState, dsp::band_splitter::Params& bandSplitParams)
    : chowdsp::EQ::EqualizerPlot (numBands,
                                  chowdsp::SpectrumPlotParams {
                                      .minFrequencyHz = 18.0f,
                                      .maxFrequencyHz = 22'000.0f,
                                      .minMagnitudeDB = -60.0f,
                                      .maxMagnitudeDB = 6.0f }),
      bandSplitterParams (bandSplitParams),
      cutoffSlider (*bandSplitParams.cutoff, *this),
      cutoffAttachment (*bandSplitParams.cutoff, pluginState, cutoffSlider)
{
    addAndMakeVisible (cutoffSlider);

    for (int bandIndex = 0; bandIndex < numBands; ++bandIndex)
        setFilterActive (bandIndex, true);

    callbacks +=
        {
            pluginState.addParameterListener (*bandSplitterParams.cutoff,
                                              chowdsp::ParameterListenerThread::MessageThread,
                                              [this]
                                              {
                                                  updateCutoffFrequency();
                                              }),
            pluginState.addParameterListener (*bandSplitterParams.slope,
                                              chowdsp::ParameterListenerThread::MessageThread,
                                              [this]
                                              {
                                                  updateFilterSlope();
                                              })
        };

    updateFilterSlope();
}

void BandSplitterPlot::updateCutoffFrequency()
{
    for (int bandIndex = 0; bandIndex < numBands; ++bandIndex)
    {
        setCutoffParameter (bandIndex, bandSplitterParams.cutoff->get());
        updateFilterPlotPath (bandIndex);
    }
}

void BandSplitterPlot::updateFilterSlope()
{
    const auto getFilterTypes = [slope = bandSplitterParams.slope->get()]
    {
        using dsp::band_splitter::Slope, chowdsp::EQ::EQPlotFilterType;
        switch (slope)
        {
            case Slope::m6_dBpOct:
                return std::make_pair (EQPlotFilterType::LPF1, EQPlotFilterType::HPF1);
            case Slope::m12_dBpOct:
                return std::make_pair (EQPlotFilterType::LPF2, EQPlotFilterType::HPF2);
            case Slope::m24_dBpOct:
                return std::make_pair (EQPlotFilterType::LPF4, EQPlotFilterType::HPF4);
            case Slope::m48_dBpOct:
                return std::make_pair (EQPlotFilterType::LPF8, EQPlotFilterType::HPF8);
            case Slope::m72_dBpOct:
                return std::make_pair (EQPlotFilterType::LPF12, EQPlotFilterType::HPF12);
        }
    };

    const auto [lowBandFilterType, highBandFilterType] = getFilterTypes();
    setFilterType (0, lowBandFilterType);
    setFilterType (1, highBandFilterType);

    for (int bandIndex = 0; bandIndex < numBands; ++bandIndex)
    {
        setCutoffParameter (bandIndex, bandSplitterParams.cutoff->get());
        setQParameter (bandIndex, 0.5f);
        updateFilterPlotPath (bandIndex);
    }
}

void BandSplitterPlot::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

void BandSplitterPlot::paintOverChildren (juce::Graphics& g)
{
    std::vector<float> freqLines { 20.0f };
    while (freqLines.back() < params.maxFrequencyHz)
    {
        const auto increment = std::pow (10.0f, std::floor (std::log10 (freqLines.back())));
        freqLines.push_back (freqLines.back() + increment);
    }

    g.setColour (juce::Colours::white.withAlpha (0.25f));
    drawFrequencyLines (g, std::move (freqLines), 1.0f);

    g.setColour (juce::Colours::white.withAlpha (0.5f));
    drawFrequencyLines (g, { 100.0f, 1'000.0f, 10'000.0f }, 1.0f);

    g.setColour (juce::Colours::white.withAlpha (0.25f));
    drawMagnitudeLines (g, { -50.0f, -40.0f, -30.0f, -20.0f, -10.0f, 0.0f });


    g.setColour (juce::Colours::red);
    g.strokePath (getPath (0), juce::PathStrokeType { 1.5f });
    g.strokePath (getPath (1), juce::PathStrokeType { 1.5f });
}

void BandSplitterPlot::resized()
{
    chowdsp::EQ::EqualizerPlot::resized();
    cutoffSlider.setBounds (getLocalBounds());
}