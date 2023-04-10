#pragma once

#include "WaveshaperDrawer.h"
#include "WaveshaperMathView.h"
#include "WaveshaperPointsView.h"
#include "dsp/Waveshaper/WaveshaperProcessor.h"
#include "state/PluginState.h"

namespace gui::waveshaper
{
class WaveshaperPlot : public juce::Slider
{
public:
    WaveshaperPlot (State& pluginState, dsp::waveshaper::Params& wsParams);

    void paint (juce::Graphics& g) override;
    void resized() override;

    void toggleDrawMode (bool isDrawMode);
    void toggleMathMode (bool isMathMode);
    void togglePointsMode (bool isPointsMode);

private:
    chowdsp::WaveshaperPlot plotter;
    WaveshaperDrawer drawArea;
    WaveshaperMathView mathArea;
    WaveshaperPointsView pointsArea;

    chowdsp::ScopedCallbackList callbacks;
    const chowdsp::EnumChoiceParameter<dsp::waveshaper::Shapes>& shapeParam;
    chowdsp::SliderAttachment gainAttach;

    bool drawMode = false;
    bool mathMode = false;
    bool pointsMode = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperPlot)
};
} // namespace gui::waveshaper
