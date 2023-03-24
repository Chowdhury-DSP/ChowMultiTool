#include "OscillatorController.h"

namespace gui::signal_gen
{
OscillatorController::OscillatorController (State& state)
    : plotSignalGen (*state.params.signalGenParams),
      gainSlider (*state.params.signalGenParams->gain,
                  state,
                  plot,
                  SpectrumDotSlider::Orientation::MagnitudeOriented),
      freqSlider (*state.params.signalGenParams->frequency,
                  state,
                  plot,
                  SpectrumDotSlider::Orientation::FrequencyOriented)
{
    gainSlider.getXCoordinate = [this, &state]
    {
        return plot.getXCoordinateForFrequency (state.params.signalGenParams->frequency->get());
    };
    addAndMakeVisible (gainSlider);

    freqSlider.getYCoordinate = [this, &state]
    {
        return plot.getYCoordinateForDecibels (state.params.signalGenParams->gain->get());
    };
    addAndMakeVisible (freqSlider);

    sliders.setSliders ({ &gainSlider, &freqSlider });
    addAndMakeVisible (sliders);

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
        state.addParameterListener (*params->frequency, listenerThread, [this]
                                    { plot.updatePlot(); }),
        state.addParameterListener (*params->gain, listenerThread, [this]
                                    { plot.updatePlot(); }),
        state.addParameterListener (*params->oscillatorChoice, listenerThread, [this]
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

    //    auto bounds = getLocalBounds();
    //    gainSlider->setBounds (bounds.removeFromTop (proportionOfHeight (0.5f)));
    //    freqSlider->setBounds (bounds);

    sliders.setBounds (getLocalBounds());
}
} // namespace gui::signal_gen
