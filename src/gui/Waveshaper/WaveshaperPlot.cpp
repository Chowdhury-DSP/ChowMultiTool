#include "WaveshaperPlot.h"
#include "gui/Shared/Colours.h"
#include "gui/Shared/LookAndFeels.h"

namespace gui::waveshaper
{
WaveshaperPlot::WaveshaperPlot (State& pluginState, dsp::waveshaper::Params& wsParams, const chowdsp::HostContextProvider& hcp)
    : plotter ({
        .xMin = -1.5f,
        .xMax = 1.5f,
    }),
      drawArea (*pluginState.nonParams.waveshaperExtraState, *pluginState.undoManager),
      mathArea (*pluginState.nonParams.waveshaperExtraState, *pluginState.undoManager),
      pointsArea (*pluginState.nonParams.waveshaperExtraState, *pluginState.undoManager),
      shapeParam (*wsParams.shapeParam),
      gainAttach (*wsParams.gainParam, pluginState, *this),
      hostContextProvider (hcp)
{
    wsParams.doForAllParameters (
        [this, &pluginState] (const auto& param, size_t)
        {
            callbacks +=
                {
                    pluginState.addParameterListener (param,
                                                      chowdsp::ParameterListenerThread::MessageThread,
                                                      [this]
                                                      {
                                                          plotter.updatePlotPath();
                                                          repaint();
                                                      }),
                };
        });

    plotter.generatePlotCallback = [this, &params = std::as_const (wsParams)]()
        -> std::pair<std::vector<float>, std::vector<float>>
    {
        using dsp::waveshaper::Shapes;
        static constexpr auto fs = 16000.0;
        static constexpr int numSamples = 4096;
        static constexpr int sineFreq = 90.0f;

        const auto linearGain = juce::Decibels::decibelsToGain (params.gainParam->get());
        plotter.params.xMin = -linearGain;
        plotter.params.xMax = linearGain;
        plotter.params.yMin = -1.1f;
        plotter.params.yMax = 1.1f;

        if (shapeParam.get() == Shapes::Free_Draw || shapeParam.get() == Shapes::Math || shapeParam.get() == Shapes::Spline)
        {
            plotter.params = spline::splineBounds;
            plotter.params.xMin = -linearGain;
            plotter.params.xMax = linearGain;
            return {};
        }

        static constexpr auto spec = juce::dsp::ProcessSpec { fs, (uint32_t) numSamples, 1 };

        chowdsp::SineWave<float> sine;
        sine.prepare (spec);
        sine.setFrequency (sineFreq);

        std::vector<float> xData, yData;
        xData.resize ((size_t) numSamples);
        yData.resize ((size_t) numSamples);

        chowdsp::BufferView<float> xBuffer { xData.data(), numSamples };
        sine.processBlock (xBuffer);
        chowdsp::BufferMath::applyGain (xBuffer, linearGain);

        if (shapeParam.get() == Shapes::Tanh_Clip)
        {
            for (auto [x, y] : chowdsp::zip (xData, yData))
                y = std::tanh (x);
        }
        else if (shapeParam.get() == Shapes::Hard_Clip)
        {
            for (auto [x, y] : chowdsp::zip (xData, yData))
                y = std::clamp (x, -1.0f, 1.0f);
        }
        else if (shapeParam.get() == Shapes::Cubic_Clip)
        {
            using chowdsp::Power::ipow;
            for (auto [x, y] : chowdsp::zip (xData, yData))
            {
                const auto x_clamp = std::clamp (x, -1.0f, 1.0f);
                y = x_clamp - ipow<3> (x_clamp) * (1.0f / 3.0f);
            }
        }
        else if (shapeParam.get() == Shapes::Nonic_Clip)
        {
            using chowdsp::Power::ipow;
            for (auto [x, y] : chowdsp::zip (xData, yData))
            {
                const auto x_clamp = std::clamp (x, -1.0f, 1.0f);
                y = x_clamp - ipow<9> (x_clamp) * (1.0f / 9.0f);
            }
        }
        else if (shapeParam.get() == Shapes::Full_Wave_Rectify)
        {
            plotter.params.yMin = -linearGain;
            plotter.params.yMax = linearGain;

            using chowdsp::Power::ipow;
            for (auto [x, y] : chowdsp::zip (xData, yData))
                y = std::max (x, 0.0f);
        }
        else if (shapeParam.get() == Shapes::Wave_Multiply)
        {
            plotter.params.yMin = -1.25f;
            plotter.params.yMax = 1.25f;

            static constexpr auto D = 2.45f;
            static constexpr auto B = 1.0f - 2.0f * 0.02f;

            juce::FloatVectorOperations::copy (yData.data(), xData.data(), numSamples);
            for (int n = 0; n < 6; ++n)
                for (auto& y : yData)
                    y = (2.0f / D) * std::tanh (y * D) - B * y;

            juce::FloatVectorOperations::multiply (yData.data(), juce::Decibels::decibelsToGain (16.0f), numSamples);
        }
        else if (shapeParam.get() == Shapes::West_Coast)
        {
            struct WCFolderCell
            {
                const double G = 0.0; // folder cell "gain"
                const double B = 0.0; // folder cell "bias"
                const double thresh = 0.0; // folder cell "threshold"
                const double mix = 0.0; // folder cell mixing factor

                const double Bp = 0.5 * G * chowdsp::Power::ipow<2> (thresh) - B * thresh;
                const double Bpp = (1.0 / 6.0) * G * chowdsp::Power::ipow<3> (thresh) - 0.5 * B * chowdsp::Power::ipow<2> (thresh) - thresh * Bp;

                [[nodiscard]] inline double operator() (double x) const noexcept
                {
                    return std::abs (x) > thresh ? (G * x - B * chowdsp::Math::sign (x)) : 0.0;
                }
            };

            static constexpr std::array<WCFolderCell, 5> cells {
                WCFolderCell { 0.8333, 0.5, 0.6, -12.0 },
                WCFolderCell { 0.3768, 1.1281, 2.994, -27.777 },
                WCFolderCell { 0.2829, 1.5446, 5.46, -21.428 },
                WCFolderCell { 0.5743, 1.0338, 1.8, 17.647 },
                WCFolderCell { 0.2673, 1.0907, 4.08, 36.363 },
            };

            for (auto [x, y] : chowdsp::zip (xData, yData))
            {
                y = 5.0f * x;
                for (auto& cell : cells)
                    y += (float) cell.mix * (float) cell ((double) x);
            }

            juce::FloatVectorOperations::multiply (yData.data(), juce::Decibels::decibelsToGain (-10.0f), numSamples);
        }
        else if (shapeParam.get() == Shapes::Fold_Fuzz)
        {
            plotter.params.yMin = 1.25f * -linearGain;
            plotter.params.yMax = 1.25f * linearGain;

            using chowdsp::Power::ipow;
            const auto k = params.kParam->get();
            const auto k_sq = ipow<2> (k);
            const auto M = params.MParam->get();
            for (auto [x, y] : chowdsp::zip (xData, yData))
                y = x * ((1.0f - k_sq) + k * std::sin (M * std::log (std::abs (x))));
        }

        return { std::vector<float> { xData.begin() + (size_t) numSamples / 2, xData.end() },
                 std::vector<float> { yData.begin() + (size_t) numSamples / 2, yData.end() } };
    };

    setTextBoxStyle (NoTextBox, false, 0, 0);
    setSliderStyle (LinearHorizontal);
    setMouseCursor (juce::MouseCursor::StandardCursorType::LeftRightResizeCursor);
    addChildComponent (drawArea);
    addChildComponent (mathArea);
    addChildComponent (pointsArea);
}

void WaveshaperPlot::toggleDrawMode (bool isDrawMode)
{
    drawMode = isDrawMode;

    drawArea.setVisible (drawMode);

    if (drawMode)
    {
        plotter.params = spline::splineBounds;
    }
    else
    {
        plotter.setSize (getLocalBounds());
    }

    repaint();
}

void WaveshaperPlot::toggleMathMode (bool isMathMode)
{
    mathMode = isMathMode;

    mathArea.setVisible (mathMode);

    if (mathMode)
    {
        plotter.params = spline::splineBounds;
    }
    else
    {
        plotter.setSize (getLocalBounds());
    }

    repaint();
}

void WaveshaperPlot::togglePointsMode (bool isPointsMode)
{
    pointsMode = isPointsMode;

    pointsArea.setVisible (pointsMode);

    if (mathMode)
    {
        plotter.params = spline::splineBounds;
    }
    else
    {
        plotter.setSize (getLocalBounds());
    }

    repaint();
}

void WaveshaperPlot::paint (juce::Graphics& g)
{
    // grid lines
    const auto drawVerticalLine = [&g, height = (float) getHeight()] (float xPos, bool major)
    {
        g.setColour (major ? colours::majorLinesColour : colours::minorLinesColour);
        g.drawLine ({ { xPos, 0.0f }, { xPos, height } });
    };
    const auto drawHorizontalLine = [&g, width = (float) getWidth()] (float yPos, bool major)
    {
        g.setColour (major ? colours::majorLinesColour : colours::minorLinesColour);
        g.drawLine ({ { 0.0f, yPos }, { width, yPos } });
    };
    const auto clampToMultiple = [] (float x, float mul)
    {
        return x + chowdsp::Math::sign (x) * std::fmod (x, mul);
    };

    drawHorizontalLine (plotter.getYCoordinateForAmplitude (0.0f), true);

    static constexpr float xInc = 0.2f;
    for (auto xPos = clampToMultiple (plotter.params.xMin, xInc); xPos < clampToMultiple (plotter.params.xMax, xInc); xPos += xInc)
    {
        const auto xPosMod = std::abs (std::fmod (xPos, 1.0f));
        const auto isMajorGridLine = juce::isWithin (xPosMod, 0.0f, 1.0e-4f) || juce::isWithin (xPosMod, 1.0f, 1.0e-4f);
        drawVerticalLine (plotter.getXCoordinateForAmplitude (xPos), isMajorGridLine);
    }
    drawVerticalLine (plotter.getXCoordinateForAmplitude (0.0f), true);

    // plot
    g.setColour (colours::plotColour);
    if (shapeParam.get() == dsp::waveshaper::Shapes::Free_Draw)
    {
        if (! drawMode)
            g.strokePath (drawArea.getDrawnPath (plotter.params), juce::PathStrokeType { 2.0f, juce::PathStrokeType::JointStyle::curved });
    }
    else if (shapeParam.get() == dsp::waveshaper::Shapes::Math)
    {
        g.strokePath (mathArea.getDrawnPath (plotter.params), juce::PathStrokeType { 2.0f, juce::PathStrokeType::JointStyle::curved });
    }
    else if (shapeParam.get() == dsp::waveshaper::Shapes::Spline)
    {
        if (! pointsMode)
            g.strokePath (pointsArea.getDrawnPath (plotter.params), juce::PathStrokeType { 2.0f, juce::PathStrokeType::JointStyle::curved });
    }
    else
    {
        g.strokePath (plotter.getPath(), juce::PathStrokeType { 2.0f, juce::PathStrokeType::JointStyle::curved });
    }
}

void WaveshaperPlot::resized()
{
    juce::Slider::resized();
    plotter.setSize (getLocalBounds());
    drawArea.setBounds (getLocalBounds());
    mathArea.setBounds (getLocalBounds());
    pointsArea.setBounds (getLocalBounds());
}

void WaveshaperPlot::mouseDown (const juce::MouseEvent& e)
{
    if (e.mods.isPopupMenu())
    {
        chowdsp::SharedLNFAllocator lnfAllocator;
        hostContextProvider.showParameterContextPopupMenu (gainAttach.getParameter(),
                                                           {},
                                                           lnfAllocator->getLookAndFeel<lnf::MenuLNF>());
        return;
    }

    juce::Slider::mouseDown (e);
}
} // namespace gui::waveshaper
