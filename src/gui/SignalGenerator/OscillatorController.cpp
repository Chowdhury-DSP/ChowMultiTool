#include "OscillatorController.h"
#include "gui/Shared/Colours.h"

namespace gui::signal_gen
{
OscillatorController::OscillatorController (State& state, const chowdsp::HostContextProvider& hcp)
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
    gainSlider.setColour (juce::Slider::thumbColourId, colours::thumbColour);
    addAndMakeVisible (gainSlider);

    freqSlider.getYCoordinate = [this, &state]
    {
        return plot.getYCoordinateForDecibels (state.params.signalGenParams->gain->get());
    };
    freqSlider.setColour (juce::Slider::thumbColourId, colours::thumbColour);
    addAndMakeVisible (freqSlider);

    sliders.setSliders ({ &gainSlider, &freqSlider });
    sliders.hostContextProvider = &hcp;
    addAndMakeVisible (sliders);

    plotSignalGen.prepare ({ OscillatorPlot::analysisFs, (uint32_t) OscillatorPlot::fftSize, 1 });

    addAndMakeVisible (plot);
    plot.toBack();
    plot.plotUpdateCallback = [this] (const chowdsp::BufferView<float>& buffer)
    {
        plotSignalGen.reset();
        plotSignalGen.processBlock (buffer);
    };

    auto& params = state.params.signalGenParams;
    plot.updatePlot (params->gain->get());

    static constexpr auto listenerThread = chowdsp::ParameterListenerThread::MessageThread;
    parameterChangeListeners += {
        state.addParameterListener (*params->frequency, listenerThread, [this, &params]
                                    { plot.updatePlot (params->gain->get()); }),
        state.addParameterListener (*params->gain, listenerThread, [this, &params]
                                    { plot.updatePlot (params->gain->get()); }),
        state.addParameterListener (*params->oscillatorChoice, listenerThread, [this, &params]
                                    { plot.updatePlot (params->gain->get()); }),
    };
}

void OscillatorController::resized()
{
    plot.setBounds (getLocalBounds());
    sliders.setBounds (getLocalBounds());
}
} // namespace gui::signal_gen
