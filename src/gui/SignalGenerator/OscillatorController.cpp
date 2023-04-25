#include "OscillatorController.h"
#include "gui/Shared/Colours.h"

namespace gui::signal_gen
{
OscillatorController::OscillatorController (State& state, const chowdsp::HostContextProvider& hcp)
    : gainSlider (*state.params.signalGenParams->gain,
                  state,
                  plot,
                  SpectrumDotSlider::Orientation::MagnitudeOriented),
      freqSlider (*state.params.signalGenParams->frequency,
                  state,
                  plot,
                  SpectrumDotSlider::Orientation::FrequencyOriented),
      chyron (state, *state.params.signalGenParams, hcp)
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

    addAndMakeVisible (plot);
    plot.toBack();

    addAndMakeVisible (chyron);

    auto& params = state.params.signalGenParams;
    plot.updatePlot (params->frequency->get(), params->gain->get(), params->oscillatorChoice->get());

    static constexpr auto listenerThread = chowdsp::ParameterListenerThread::MessageThread;
    parameterChangeListeners += {
        state.addParameterListener (*params->frequency, listenerThread, [this, &params]
                                    { plot.updatePlot (params->frequency->get(), params->gain->get(), params->oscillatorChoice->get()); }),
        state.addParameterListener (*params->gain, listenerThread, [this, &params]
                                    { plot.updatePlot (params->frequency->get(), params->gain->get(), params->oscillatorChoice->get()); }),
        state.addParameterListener (*params->oscillatorChoice, listenerThread, [this, &params]
                                    { plot.updatePlot (params->frequency->get(), params->gain->get(), params->oscillatorChoice->get()); }),
    };
}

void OscillatorController::resized()
{
    plot.setBounds (getLocalBounds());
    sliders.setBounds (getLocalBounds());

    const auto pad = proportionOfWidth (0.005f);
    const auto chyronWidth = proportionOfWidth (0.2f);
    const auto chyronHeight = proportionOfWidth (0.1f);
    chyron.setBounds (pad,
                      getHeight() - 8 * pad - chyronHeight,
                      chyronWidth,
                      chyronHeight);
}
} // namespace gui::signal_gen
