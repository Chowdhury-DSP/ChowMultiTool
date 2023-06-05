#include "BandSplitterPlot.h"
#include "gui/Shared/Colours.h"
#include "gui/Shared/FrequencyPlotHelpers.h"
#include "gui/Shared/LookAndFeels.h"

namespace gui::band_splitter
{
namespace
{
    constexpr int numBands = 4;
    constexpr int minFrequency = 18;
    constexpr int maxFrequency = 22'000;
} // namespace

BandSplitterPlot::InternalSlider::InternalSlider (chowdsp::FloatParameter& cutoff,
                                                  chowdsp::EQ::EqualizerPlot& plot,
                                                  State& pluginState,
                                                  const chowdsp::HostContextProvider& hcp)
    : cutoffParam (cutoff),
      plotBase (plot),
      cutoffAttachment (cutoff, pluginState, *this),
      hostContextProvider (hcp)
{
    setTextBoxStyle (NoTextBox, false, 0, 0);
    setSliderStyle (LinearHorizontal);
    setMouseCursor (juce::MouseCursor::StandardCursorType::LeftRightResizeCursor);
}

void BandSplitterPlot::InternalSlider::paint (juce::Graphics& g)
{
    const auto thumbBounds = getThumbBounds();
    const auto thumbBoundsFloat = thumbBounds.toFloat();
    juce::ColourGradient grad { colours::thumbColour.withAlpha (0.4f),
                                juce::Point { thumbBoundsFloat.getX(), thumbBoundsFloat.getHeight() * 0.5f },
                                colours::thumbColour.withAlpha (0.4f),
                                juce::Point { thumbBoundsFloat.getRight(), thumbBoundsFloat.getHeight() * 0.5f },
                                false };
    grad.addColour (0.5, colours::thumbColour.withAlpha (0.8f));
    g.setGradientFill (std::move (grad));
    g.fillRect (thumbBounds);
}

bool BandSplitterPlot::InternalSlider::hitTest (int x, int y)
{
    return getThumbBounds().contains (x, y);
}

void BandSplitterPlot::InternalSlider::mouseDown (const juce::MouseEvent& e)
{
    if (e.mods.isPopupMenu())
    {
        chowdsp::SharedLNFAllocator lnfAllocator;
        hostContextProvider.showParameterContextPopupMenu (cutoffParam,
                                                           {},
                                                           lnfAllocator->getLookAndFeel<lnf::MenuLNF>());
        return;
    }
    juce::Slider::mouseDown (e);
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

BandSplitterPlot::BandSplitterPlot (State& pluginState, dsp::band_splitter::Params& bandSplitParams, const chowdsp::HostContextProvider& hcp)
    : chowdsp::EQ::EqualizerPlot (numBands,
                                  chowdsp::SpectrumPlotParams {
                                      .minFrequencyHz = (float) minFrequency,
                                      .maxFrequencyHz = (float) maxFrequency,
                                      .minMagnitudeDB = -60.0f,
                                      .maxMagnitudeDB = 6.0f }),
      bandSplitterParams (bandSplitParams),
      cutoffSlider (*bandSplitParams.cutoff, *this, pluginState, hcp),
      cutoff2Slider (*bandSplitParams.cutoff2, *this, pluginState, hcp)
{
    addAndMakeVisible (cutoffSlider);
    addChildComponent (cutoff2Slider);
    cutoff2Slider.setVisible (bandSplitterParams.threeBandOnOff->get());

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
            pluginState.addParameterListener (*bandSplitterParams.cutoff2,
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
                                              }),
            pluginState.addParameterListener (*bandSplitterParams.threeBandOnOff,
                                              chowdsp::ParameterListenerThread::MessageThread,
                                              [this]
                                              {
                                                  cutoff2Slider.setVisible (bandSplitterParams.threeBandOnOff->get());
                                              }),
        };

    updateFilterSlope();
}

void BandSplitterPlot::updateCutoffFrequency()
{
    for (int bandIndex = 0; bandIndex < numBands; ++bandIndex)
    {
        const auto& cutoffParam = bandIndex < (numBands / 2) ? bandSplitterParams.cutoff : bandSplitterParams.cutoff2;
        setCutoffParameter (bandIndex, cutoffParam->get());
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
    setFilterType (2, lowBandFilterType);
    setFilterType (3, highBandFilterType);

    for (int bandIndex = 0; bandIndex < numBands; ++bandIndex)
    {
        const auto& cutoffParam = bandIndex < (numBands / 2) ? bandSplitterParams.cutoff : bandSplitterParams.cutoff2;
        setCutoffParameter (bandIndex, cutoffParam->get());
        setQParameter (bandIndex, 0.5f);
        updateFilterPlotPath (bandIndex);
    }
}

void BandSplitterPlot::paintOverChildren (juce::Graphics& g)
{
    drawMagnitudeLabels (g, *this, { -50.0f, -40.0f, -30.0f, -20.0f, -10.0f, 0.0f });
    drawFrequencyLabels (g, *this, { 100.0f, 1'000.0f, 10'000.0f }, 2.0f);
    gui::drawFrequencyLines<minFrequency, maxFrequency> (*this,
                                                         g,
                                                         { 100.0f, 1'000.0f, 10'000.0f },
                                                         colours::majorLinesColour,
                                                         colours::minorLinesColour);
    gui::drawMagnitudeLines (*this,
                             g,
                             { -50.0f, -40.0f, -30.0f, -20.0f, -10.0f, 0.0f },
                             { 0.0f },
                             colours::majorLinesColour,
                             colours::minorLinesColour);

    g.setColour (colours::plotColour);
    g.strokePath (getPath (0), juce::PathStrokeType { 2.0f });
    g.strokePath (getPath (1), juce::PathStrokeType { 2.0f });

    if (bandSplitterParams.threeBandOnOff->get())
    {
        g.strokePath (getPath (2), juce::PathStrokeType { 2.0f });
        g.strokePath (getPath (3), juce::PathStrokeType { 2.0f });
    }
}

void BandSplitterPlot::resized()
{
    chowdsp::EQ::EqualizerPlot::resized();

    const auto bounds = getLocalBounds();
    cutoffSlider.setBounds (bounds);
    cutoff2Slider.setBounds (bounds);
}
} // namespace gui::band_splitter
