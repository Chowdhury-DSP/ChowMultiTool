#include "WaveshaperPlot.h"

namespace gui::waveshaper
{
WaveshaperPlot::WaveshaperPlot (State& pluginState, dsp::waveshaper::Params& wsParams)
    : plotter ({
        .xMin = -1.5f,
        .xMax = 1.5f,
    })
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

    plotter.generatePlotCallback = [&params = std::as_const (wsParams)]()
        -> std::pair<std::vector<float>, std::vector<float>>
    {
        static constexpr auto fs = 16000.0;
        static constexpr int numSamples = 4096;
        static constexpr int sineFreq = 90.0f;

        static constexpr auto spec = juce::dsp::ProcessSpec { fs, (uint32_t) numSamples, 1 };

        chowdsp::SineWave<float> sine;
        sine.prepare (spec);
        sine.setFrequency (sineFreq);

        std::vector<float> xData, yData;
        xData.resize ((size_t) numSamples);
        yData.resize ((size_t) numSamples);

        chowdsp::BufferView<float> xBuffer { xData.data(), numSamples };
        sine.processBlock (xBuffer);
        chowdsp::BufferMath::applyGain (xBuffer, juce::Decibels::decibelsToGain (params.gainParam->get()));

        using dsp::waveshaper::Shapes;
        if (params.shapeParam->get() == Shapes::Tanh_Clip)
        {
            for (auto [x, y] : chowdsp::zip (xData, yData))
                y = std::tanh (x);
        }
        else if (params.shapeParam->get() == Shapes::Hard_Clip)
        {
            for (auto [x, y] : chowdsp::zip (xData, yData))
                y = std::clamp (x, -1.0f, 1.0f);
        }
        else if (params.shapeParam->get() == Shapes::Cubic_Clip)
        {
            using chowdsp::Power::ipow;
            for (auto [x, y] : chowdsp::zip (xData, yData))
            {
                const auto x_clamp = std::clamp (x, -1.0f, 1.0f);
                y = x_clamp - ipow<3> (x_clamp) * (1.0f / 3.0f);
            }
        }
        else if (params.shapeParam->get() == Shapes::Nonic_Clip)
        {
            using chowdsp::Power::ipow;
            for (auto [x, y] : chowdsp::zip (xData, yData))
            {
                const auto x_clamp = std::clamp (x, -1.0f, 1.0f);
                y = x_clamp - ipow<9> (x_clamp) * (1.0f / 9.0f);
            }
        }
        else if (params.shapeParam->get() == Shapes::Full_Wave_Rectify)
        {
            using chowdsp::Power::ipow;
            for (auto [x, y] : chowdsp::zip (xData, yData))
                y = std::max (x, 0.0f);
        }
        else if (params.shapeParam->get() == Shapes::Wave_Multiply)
        {
            static constexpr auto D = 2.45f;
            static constexpr auto B = 1.0f - 2.0f * 0.02f;

            juce::FloatVectorOperations::copy (yData.data(), xData.data(), numSamples);
            for (int n = 0; n < 6; ++n)
                for (auto& y : yData)
                    y = (2.0f / D) * std::tanh (y * D) - B * y;
        }
        else if (params.shapeParam->get() == Shapes::West_Coast)
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
        }
        else if (params.shapeParam->get() == Shapes::Fold_Fuzz)
        {
            using chowdsp::Power::ipow;
            const auto k = params.kParam->get();
            const auto k_sq = ipow<2> (k);
            const auto M = params.MParam->get();
            for (auto [x, y] : chowdsp::zip (xData, yData))
                y = x * ((1.0f - k_sq) + k * std::sin (M * std::log (std::abs (x))));
        }

        // @TODO: make this look better!
        return { std::vector<float> { xData.begin() + (size_t) numSamples / 2, xData.end() },
                 std::vector<float> { yData.begin() + (size_t) numSamples / 2, yData.end() } };
    };
}

void WaveshaperPlot::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

    g.setColour (juce::Colours::red);
    g.strokePath (plotter.getPath(), juce::PathStrokeType { 2.0f, juce::PathStrokeType::JointStyle::curved });
}

void WaveshaperPlot::resized()
{
    plotter.setSize (getLocalBounds());
}
} // namespace gui::waveshaper
