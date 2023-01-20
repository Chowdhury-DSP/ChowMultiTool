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
    plotSignalGen.prepare ({ OscillatorPlot::analysisFs, (uint32_t) OscillatorPlot::fftSize, 1 });

    addAndMakeVisible (plot);
    plot.toBack();
    plot.plotUpdateCallback = [this] (const chowdsp::BufferView<float>& buffer)
    {
        plotSignalGen.reset();
        plotSignalGen.processBlock (buffer);
    };
    plot.updatePlot();

    auto& params = state.params.signalGenParams;
    static constexpr auto listenerThread = chowdsp::ParameterListenerThread::MessageThread;
    parameterChangeListeners += {
        state.addParameterListener (*params.frequency, listenerThread, [this]
                                    { plot.updatePlot(); }),
        state.addParameterListener (*params.gain, listenerThread, [this]
                                    { plot.updatePlot(); }),
        state.addParameterListener (*params.oscillatorChoice, listenerThread, [this]
                                    { plot.updatePlot(); }),
    };
}

void OscillatorController::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

    //    g.setColour (juce::Colours::red);
    //    plot.drawPlot (g);
}

void OscillatorController::resized()
{
    plot.setBounds (getLocalBounds());

    auto bounds = getLocalBounds();
    gainSlider->setBounds (bounds.removeFromTop (proportionOfHeight (0.5f)));
    freqSlider->setBounds (bounds);
}
} // namespace gui::signal_gen
