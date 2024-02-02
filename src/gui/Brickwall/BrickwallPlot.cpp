#include "BrickwallPlot.h"
#include "gui/Shared/Colours.h"
#include "gui/Shared/FrequencyPlotHelpers.h"
#include "gui/Shared/LookAndFeels.h"

namespace
{
#if JUCE_DEBUG
constexpr int fftOrder = 12;
#else
constexpr int fftOrder = 15;
#endif

constexpr double sampleRate = 48000.0f;
constexpr int blockSize = 1 << fftOrder;

constexpr int minFrequency = 18;
constexpr int maxFrequency = 22'000;
} // namespace

namespace gui::brickwall
{
BrickwallPlot::InternalSlider::InternalSlider (chowdsp::FloatParameter& cutoff,
                                               chowdsp::SpectrumPlotBase& plot,
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

void BrickwallPlot::InternalSlider::paint (juce::Graphics& g)
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

bool BrickwallPlot::InternalSlider::hitTest (int x, int y)
{
    return getThumbBounds().contains (x, y);
}

void BrickwallPlot::InternalSlider::mouseDown (const juce::MouseEvent& e)
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

double BrickwallPlot::InternalSlider::proportionOfLengthToValue (double proportion)
{
    return (double) plotBase.getFrequencyForXCoordinate ((float) proportion * (float) getWidth());
}

double BrickwallPlot::InternalSlider::valueToProportionOfLength (double value)
{
    return (double) plotBase.getXCoordinateForFrequency ((float) value) / (double) getWidth();
}

juce::Rectangle<int> BrickwallPlot::InternalSlider::getThumbBounds() const
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

//==============================================================================
void BrickwallPlot::FilterPlotComp::paint (juce::Graphics& g)
{
    g.setColour (colours::plotColour);
    g.strokePath (parent->filterPlotter.getPath(), juce::PathStrokeType { 2.0f });
}

//==============================================================================

BrickwallPlot::BrickwallPlot (State& pluginState,
                              dsp::brickwall::Params& brickwallParams,
                              dsp::brickwall::ExtraState& brickwallExtraState,
                              const chowdsp::HostContextProvider& hcp,
                              SpectrumAnalyserTask::PrePostPair spectrumAnalyserTasks)
    : chowdsp::SpectrumPlotBase (chowdsp::SpectrumPlotParams {
        .minFrequencyHz = (float) minFrequency,
        .maxFrequencyHz = (float) maxFrequency,
        .minMagnitudeDB = -60.0f,
        .maxMagnitudeDB = 6.0f,
    }),
      filterPlotter (*this, chowdsp::GenericFilterPlotter::Params {
                                .sampleRate = sampleRate,
                                .fftOrder = fftOrder,
                            }),
      brickwall (brickwallParams, *pluginState.nonParams.brickwallExtraState),
      extraState (brickwallExtraState),
      spectrumAnalyser (*this, spectrumAnalyserTasks),
      chyron (pluginState, brickwallParams, hcp),
      cutoffSlider (*brickwallParams.cutoff, *this, pluginState, hcp)
{
    extraState.isEditorOpen.store (true);
    spectrumAnalyser.setShouldShowPostEQ (extraState.showSpectrum.get());
    spectrumAnalyser.postEQDrawOptions.gradientStartColour = juce::Colour { 0xff008080 }.withAlpha (0.5f);
    spectrumAnalyser.postEQDrawOptions.gradientEndColour = juce::Colour { 0xff00008b }.withAlpha (0.5f);
    callbacks += {
        extraState.showSpectrum.changeBroadcaster.connect (
            [this]
            {
                spectrumAnalyser.setShouldShowPostEQ (extraState.showSpectrum.get());
                spectrumAnalyser.repaint();
            }),
    };
    addAndMakeVisible (spectrumAnalyser);

    brickwall.prepare ({ sampleRate, (uint32_t) blockSize, 1 });
    filterPlotter.runFilterCallback = [this] (const float* input, float* output, int numSamples)
    {
        brickwall.reset();
        juce::FloatVectorOperations::copy (output, input, numSamples);
        brickwall.processBlock (chowdsp::BufferView<float> { output, numSamples });
    };

    brickwallParams.doForAllParameters (
        [this, &pluginState] (juce::RangedAudioParameter& param, size_t)
        {
            callbacks +=
                {
                    pluginState.addParameterListener (
                        param,
                        chowdsp::ParameterListenerThread::MessageThread,
                        [this]
                        { updatePlot(); }),
                };
        });

    updatePlot();
    addAndMakeVisible (cutoffSlider);
    plotComp.setInterceptsMouseClicks (false, false);
    plotComp.parent = this;
    addAndMakeVisible (plotComp);
    addAndMakeVisible (chyron);
}

BrickwallPlot::~BrickwallPlot()
{
    extraState.isEditorOpen.store (false);
}

void BrickwallPlot::updatePlot()
{
    filterPlotter.updateFilterPlot();
    repaint();
}

void BrickwallPlot::mouseDown (const juce::MouseEvent& event)
{
    if (event.mods.isPopupMenu())
    {
        chowdsp::SharedLNFAllocator lnfAllocator;
        juce::PopupMenu menu;

        juce::PopupMenu::Item postSpectrumItem;
        postSpectrumItem.itemID = 100;
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

void BrickwallPlot::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

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

void BrickwallPlot::resized()
{
    updatePlot();
    spectrumAnalyser.setBounds (getLocalBounds());
    plotComp.setBounds (getLocalBounds());
    cutoffSlider.setBounds (getLocalBounds());
    const auto pad = proportionOfWidth (0.005f);
    const auto chyronWidth = proportionOfWidth (0.15f);
    const auto chyronHeight = proportionOfWidth (0.05f);
    chyron.setBounds (getWidth() - pad - chyronWidth,
                      getHeight() - pad - proportionOfHeight (0.075f) - chyronHeight,
                      chyronWidth,
                      chyronHeight);
}
} // namespace gui::brickwall
