#include "OscillatorController.h"

namespace gui::signal_gen
{
struct DraggingSlider : public juce::Slider
{
    DraggingSlider (chowdsp::FloatParameter& param,
                    State& state,
                    juce::MouseCursor&& mouseCursor)
        : attachment (param, state, *this),
          cursor (std::move (mouseCursor))
    {
        setTextBoxStyle (NoTextBox, false, 0, 0);
    }

    void paint (juce::Graphics&) override {}

    void mouseMove (const juce::MouseEvent& e) override
    {
        setMouseCursor (cursor);
        juce::Slider::mouseMove (e);
    }

    chowdsp::SliderAttachment attachment;
    juce::MouseCursor cursor;
};

OscillatorController::OscillatorController (State& state)
{
    gainSlider = std::make_unique<DraggingSlider> (*state.params.signalGenParams.gain, state, juce::MouseCursor::StandardCursorType::UpDownResizeCursor);
    gainSlider->setSliderStyle (juce::Slider::LinearVertical);
    addAndMakeVisible (gainSlider.get());

    freqSlider = std::make_unique<DraggingSlider> (*state.params.signalGenParams.frequency, state, juce::MouseCursor::StandardCursorType::LeftRightResizeCursor);
    freqSlider->setSliderStyle (juce::Slider::LinearHorizontal);
    addAndMakeVisible (freqSlider.get());

    plotSignalGen.initialise (state.params);

    static constexpr float fs = 480000.0f;
    static constexpr float secondsToShow = 0.001f;
    plotSignalGen.prepare ({ 48000.0, (uint32_t) juce::roundToInt (fs * secondsToShow), 1 });

    plot.updatePlotConfig (fs, secondsToShow);
    plot.plotUpdateCallback = [this] (const chowdsp::BufferView<float>& buffer)
    {
        plotSignalGen.reset();
        plotSignalGen.processBlock (buffer);
    };

    parameterChangeListeners += {
        state.addParameterListener (*state.params.signalGenParams.frequency, chowdsp::ParameterListenerThread::MessageThread, [this] { plot.updatePlot (this); }),
        state.addParameterListener (*state.params.signalGenParams.gain, chowdsp::ParameterListenerThread::MessageThread, [this] { plot.updatePlot (this); }),
        state.addParameterListener (*state.params.signalGenParams.oscillatorChoice, chowdsp::ParameterListenerThread::MessageThread, [this] { plot.updatePlot (this); }),
    };
}

void OscillatorController::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

    g.setColour (juce::Colours::red);
    plot.drawPlot (g);
}

void OscillatorController::resized()
{
    plot.updateSize (getWidth(), getHeight());

    auto bounds = getLocalBounds();
    gainSlider->setBounds (bounds.removeFromTop (proportionOfHeight (0.5f)));
    freqSlider->setBounds (bounds);
}
} // namespace gui::signal_gen
