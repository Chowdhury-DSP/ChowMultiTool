#include "WaveshaperPlot.h"

namespace gui::waveshaper
{
WaveshaperPlot::WaveshaperPlot (State& pluginState, dsp::waveshaper::Params& wsParams)
    : processor (pluginState, wsParams),
      plotter ({
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

    plotter.generatePlotCallback = [this]() -> std::pair<std::vector<float>, std::vector<float>>
    {
        static constexpr auto fs = 16000.0;
        static constexpr int numSamples = 1 << 10;
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
        chowdsp::BufferMath::applyGain (xBuffer, plotter.params.xMax);

        chowdsp::BufferView<float> yBuffer { yData.data(), numSamples };
        chowdsp::BufferMath::copyBufferData (xBuffer, yBuffer);

        processor.prepare (spec);
        processor.processBlock (yBuffer);

        // @TODO: get latency from oversampling parameter
        // and get the visualizer to match up accordingly
        const int latencyOffset = 0;
        return { std::vector<float> { xData.begin() + (size_t) numSamples / 2 + (size_t) latencyOffset, xData.end() },
                 std::vector<float> { yData.begin() + (size_t) numSamples / 2, yData.end() - (size_t) latencyOffset } };
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
