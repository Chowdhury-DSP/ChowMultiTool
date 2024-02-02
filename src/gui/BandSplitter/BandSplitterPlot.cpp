#include "BandSplitterPlot.h"
#include "gui/Shared/Colours.h"
#include "gui/Shared/FrequencyPlotHelpers.h"
#include "gui/Shared/LookAndFeels.h"

namespace gui::band_splitter
{
namespace
{
    constexpr int numBands = 6;
    constexpr int minFrequency = 18;
    constexpr int maxFrequency = 22'000;
    using BandState = dsp::band_splitter::BandState;
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

BandSplitterPlot::BandSplitterPlot (State& pluginState,
                                    dsp::band_splitter::Params& bandSplitParams,
                                    dsp::band_splitter::ExtraState& bandSplitterExtraState,
                                    const chowdsp::HostContextProvider& hcp,
                                    dsp::band_splitter::BandSplitterSpectrumTasks& splitterSpectrumTasks)

    : chowdsp::EQ::EqualizerPlot (numBands,
                                  chowdsp::SpectrumPlotParams {
                                      .minFrequencyHz = (float) minFrequency,
                                      .maxFrequencyHz = (float) maxFrequency,
                                      .minMagnitudeDB = -60.0f,
                                      .maxMagnitudeDB = 6.0f }),
      bandSplitterParams (bandSplitParams),
      extraState (bandSplitterExtraState),
      cutoffSlider (*bandSplitParams.cutoff, *this, pluginState, hcp),
      cutoff2Slider (*bandSplitParams.cutoff2, *this, pluginState, hcp),
      cutoff3Slider (*bandSplitParams.cutoff3, *this, pluginState, hcp),
      spectrumTasks (splitterSpectrumTasks),
      chyron (pluginState, bandSplitParams, hcp)
{
    extraState.isEditorOpen.store (true);

    addAndMakeVisible (cutoffSlider);
    addChildComponent (cutoff2Slider);
    addChildComponent (cutoff3Slider);
    cutoff2Slider.setVisible (bandSplitterParams.threeBandOnOff->get());
    cutoff3Slider.setVisible (bandSplitterParams.fourBandOnOff->get());

    plotComp.setInterceptsMouseClicks (false, false);
    plotComp.parent = this;
    addAndMakeVisible (plotComp);

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
            pluginState.addParameterListener (*bandSplitterParams.cutoff3,
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
                                                  auto bandState = bandSplitterParams.getCurrentBandState();
                                                  cutoff2Slider.setVisible (bandState == BandState::ThreeBands || bandState == BandState::FourBands);
                                                  cutoff3Slider.setVisible (bandState == BandState::FourBands);
                                                  updateSpectrumPlots();
                                                  repaint();
                                              }),
            pluginState.addParameterListener (*bandSplitterParams.fourBandOnOff,
                                              chowdsp::ParameterListenerThread::MessageThread,
                                              [this]
                                              {
                                                  auto bandState = bandSplitterParams.getCurrentBandState();
                                                  cutoff2Slider.setVisible (bandState == BandState ::FourBands || bandState == BandState::ThreeBands);
                                                  cutoff3Slider.setVisible (bandState == BandState::FourBands);
                                                  updateSpectrumPlots();
                                                  repaint();
                                              }),
        };

    callbacks += {
        extraState.showSpectrum.changeBroadcaster.connect ([this]
                                                           {
                                                                   for (auto [_, spectrum] : spectrumAnalysers)
                                                                   {
                                                                       spectrum->setShouldShowPostEQ(extraState.showSpectrum.get());
                                                                       spectrum->repaint();
                                                                   } }),
    };

    addAndMakeVisible (chyron);
    chyron.toFront (false);

    updateFilterSlope();
    updateSpectrumPlots();
}

BandSplitterPlot::~BandSplitterPlot()
{
    extraState.isEditorOpen.store (false);
}

void BandSplitterPlot::updateCutoffFrequency()
{
    for (int bandIndex = 0; bandIndex < numBands; ++bandIndex) //bands 0, 1, 2, 3, 4, 5
    {
        setCutoffParameter (bandIndex, getCutoffParam (bandIndex, bandSplitterParams)->get());
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
    setFilterType (4, lowBandFilterType);
    setFilterType (5, highBandFilterType);

    for (int bandIndex = 0; bandIndex < numBands; ++bandIndex)
    {
        setCutoffParameter (bandIndex, getCutoffParam (bandIndex, bandSplitterParams)->get());
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
}

void BandSplitterPlot::resized()
{
    chowdsp::EQ::EqualizerPlot::resized();

    plotComp.setBounds (getLocalBounds());
    const auto pad = proportionOfWidth (0.005f);
    const auto chyronWidth = proportionOfWidth (0.20f);
    const auto chyronHeight = proportionOfWidth (0.1f);
    chyron.setBounds (getWidth() - pad - chyronWidth,
                      getHeight() - pad - chyronHeight - proportionOfHeight (0.075f),
                      chyronWidth,
                      chyronHeight);

    const auto bounds = getLocalBounds();
    for (auto [_, spectrum] : spectrumAnalysers)
        spectrum->setBounds (bounds);
    cutoffSlider.setBounds (bounds);
    cutoff2Slider.setBounds (bounds);
    cutoff3Slider.setBounds (bounds);
}

void BandSplitterPlot::mouseDown (const juce::MouseEvent& event)
{
    if (event.mods.isPopupMenu())
    {
        chowdsp::SharedLNFAllocator lnfAllocator;
        juce::PopupMenu menu;

        juce::PopupMenu::Item postSpectrumItem;
        postSpectrumItem.itemID = 101;
        postSpectrumItem.text = extraState.showSpectrum.get() ? "Disable Spectrum Visualizer" : "Enable Spectrum Visualizer";
        postSpectrumItem.action = [this]
        {
            extraState.showSpectrum.set (! extraState.showSpectrum.get());
        };
        menu.addItem (postSpectrumItem);

        menu.setLookAndFeel (lnfAllocator->getLookAndFeel<lnf::MenuLNF>());
        menu.showMenuAsync (juce::PopupMenu::Options {}
                                .withParentComponent (getParentComponent()));
    }
}

void BandSplitterPlot::updateSpectrumPlots()
{
    spectrumAnalysers.clear();
    spectrumAnalysers.reserve (4);

    using dsp::band_splitter::SpectrumBandID;
    const auto spectrumIDs = [this]
    {
        using IDList = chowdsp::SmallVector<SpectrumBandID, 4>;
        if (bandSplitterParams.fourBandOnOff->get())
            return IDList { SpectrumBandID::Low, SpectrumBandID::LowMid, SpectrumBandID::HighMid, SpectrumBandID::High };
        else if (bandSplitterParams.threeBandOnOff->get())
            return IDList { SpectrumBandID::Low, SpectrumBandID::Mid, SpectrumBandID::High };
        return IDList { SpectrumBandID::Low, SpectrumBandID::High };
    };

    for (auto id : spectrumIDs())
    {
        auto* spectrumTask = *spectrumTasks[id];
        auto prePostPair = SpectrumAnalyserTask::PrePostPair { std::nullopt, std::ref (spectrumTask->spectrumAnalyserUITask) };
        spectrumAnalysers.insert_or_assign (id, std::make_unique<SpectrumAnalyser> (*this, prePostPair));
    }

    setSpectrumColours();

    for (auto [_, spectrum] : spectrumAnalysers)
    {
        spectrum->setShouldShowPostEQ (extraState.showSpectrum.get());
        addAndMakeVisible (*spectrum);
        spectrum->toBack();
    }
    resized();
}

void BandSplitterPlot::setSpectrumColours()
{
    using dsp::band_splitter::SpectrumBandID;
    for (auto [id, analyser] : spectrumAnalysers)
    {
        analyser->postEQDrawOptions.drawLine = true;
        switch (id)
        {
            case SpectrumBandID::Low:
                analyser->postEQDrawOptions.gradientEndColour = juce::Colour (0xff00008b).withAlpha (0.4f);
                analyser->postEQDrawOptions.gradientStartColour = juce::Colour (0xff008080).withAlpha (0.4f);
                analyser->postEQDrawOptions.lineColour = juce::Colour (0xff008080).brighter();
                break;
            case SpectrumBandID::Mid:
                analyser->postEQDrawOptions.gradientEndColour = juce::Colour::fromRGB (0xFF, 0x66, 0x00).withAlpha (0.4f);
                analyser->postEQDrawOptions.gradientStartColour = juce::Colour::fromRGB (255, 215, 0).withAlpha (0.4f);
                analyser->postEQDrawOptions.lineColour = juce::Colour::fromRGB (255, 215, 0).brighter();
                break;
            case SpectrumBandID::High:
                analyser->postEQDrawOptions.gradientEndColour = juce::Colour::fromRGB (0x8A, 0x2B, 0xE2).withAlpha (0.4f);
                analyser->postEQDrawOptions.gradientStartColour = juce::Colour::fromRGB (0xDA, 0x70, 0xD6).withAlpha (0.4f);
                analyser->postEQDrawOptions.lineColour = juce::Colour::fromRGB (0x8A, 0x2B, 0xE2).brighter();
                break;
            case SpectrumBandID::LowMid:
                analyser->postEQDrawOptions.gradientEndColour = juce::Colour::fromRGB (0xFF, 0x66, 0x00).withAlpha (0.4f);
                analyser->postEQDrawOptions.gradientStartColour = juce::Colour::fromRGB (255, 215, 0).withAlpha (0.4f);
                analyser->postEQDrawOptions.lineColour = juce::Colour::fromRGB (255, 215, 0).brighter();
                break;
            case SpectrumBandID::HighMid:
                analyser->postEQDrawOptions.gradientEndColour = juce::Colour::fromRGB (0x00, 0xFF, 0x7F).withAlpha (0.4f);
                analyser->postEQDrawOptions.gradientStartColour = juce::Colour::fromRGB (0x00, 0x80, 0x80).withAlpha (0.4f);
                analyser->postEQDrawOptions.lineColour = juce::Colour::fromRGB (0x00, 0xE5, 0xFF).brighter();
                break;
        }
    }
}
const chowdsp::FreqHzParameter::Ptr& BandSplitterPlot::getCutoffParam (int bandIndex, const dsp::band_splitter::Params& bandParams)
{
    if (bandIndex < (numBands / 3))
        return bandParams.cutoff;
    else if (bandIndex <= numBands / 2)
        return bandParams.cutoff2;
    else
        return bandParams.cutoff3;
};

void BandSplitterPlot::FilterPlotComp::paint (juce::Graphics& g)
{
    g.setColour (colours::plotColour);
    g.strokePath (parent->getPath (0), juce::PathStrokeType { 2.0f });
    g.strokePath (parent->getPath (1), juce::PathStrokeType { 2.0f });

    auto bandState = parent->bandSplitterParams.getCurrentBandState();
    if (bandState == BandState::FourBands || bandState == BandState::ThreeBands)
    {
        g.strokePath (parent->getPath (2), juce::PathStrokeType { 2.0f });
        g.strokePath (parent->getPath (3), juce::PathStrokeType { 2.0f });
    }
    if (bandState == BandState::FourBands)
    {
        g.strokePath (parent->getPath (4), juce::PathStrokeType { 2.0f });
        g.strokePath (parent->getPath (5), juce::PathStrokeType { 2.0f });
    }
}
} // namespace gui::band_splitter
