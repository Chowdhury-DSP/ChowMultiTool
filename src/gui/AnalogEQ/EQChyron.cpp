#include "EQChyron.h"
#include "gui/Shared/Colours.h"

namespace gui::analog_eq
{
constexpr float labelHeightFactor = 0.275f;

EQChyron::EQChyron (chowdsp::PluginState& state, dsp::analog_eq::Params& params)
    : lowFreqBoostSlider (state, params.bassBoostParam.get()),
      lowFreqCutSlider (state, params.bassCutParam.get()),
      lowFreqCutoffSlider (state, params.bassFreqParam.get()),
      highFreqBoostSlider (state, params.trebleBoostParam.get()),
      highFreqBoostFreqSlider (state, params.trebleBoostFreqParam.get()),
      highFreqBoostQSlider (state, params.trebleBoostQParam.get()),
      highFreqCutSlider (state, params.trebleCutParam.get()),
      highFreqCutFreqSlider (state, params.trebleCutFreqParam.get())
{
    for (auto* param : { (chowdsp::FloatParameter*) params.bassBoostParam.get(),
                         (chowdsp::FloatParameter*) params.bassCutParam.get(),
                         (chowdsp::FloatParameter*) params.bassFreqParam.get() })
    {
        callbacks += {
            state.addParameterListener (*param,
                                        chowdsp::ParameterListenerThread::MessageThread,
                                        [this]
                                        { setSelectedBand (EQBand::Bass); }),
        };
    }

    for (auto* param : { (chowdsp::FloatParameter*) params.trebleCutParam.get(),
                         (chowdsp::FloatParameter*) params.trebleCutFreqParam.get() })
    {
        callbacks += {
            state.addParameterListener (*param,
                                        chowdsp::ParameterListenerThread::MessageThread,
                                        [this]
                                        { setSelectedBand (EQBand::TrebleCut); }),
        };
    }

    for (auto* param : { (chowdsp::FloatParameter*) params.trebleBoostParam.get(),
                         (chowdsp::FloatParameter*) params.trebleBoostFreqParam.get(),
                         (chowdsp::FloatParameter*) params.trebleBoostQParam.get() })
    {
        callbacks += {
            state.addParameterListener (*param,
                                        chowdsp::ParameterListenerThread::MessageThread,
                                        [this]
                                        { setSelectedBand (EQBand::TrebleBoost); }),
        };
    }

    lowFreqBoostSlider.setName ("Boost");
    addAndMakeVisible (lowFreqBoostSlider);
    lowFreqCutSlider.setName ("Cut");
    addAndMakeVisible (lowFreqCutSlider);
    lowFreqCutoffSlider.setName ("Cutoff");
    addAndMakeVisible (lowFreqCutoffSlider);

    highFreqBoostSlider.setName ("Boost");
    addChildComponent (highFreqBoostSlider);
    highFreqBoostFreqSlider.setName ("Cutoff");
    addChildComponent (highFreqBoostFreqSlider);
    highFreqBoostQSlider.setName ("Q");
    addChildComponent (highFreqBoostQSlider);

    highFreqCutSlider.setName ("Cut");
    addChildComponent (highFreqCutSlider);
    highFreqCutFreqSlider.setName ("Cutoff");
    addChildComponent (highFreqCutFreqSlider);

    setSelectedBand (band);
}

void EQChyron::setSelectedBand (EQBand eqBand)
{
    band = eqBand;

    lowFreqBoostSlider.setVisible (false);
    lowFreqCutSlider.setVisible (false);
    lowFreqCutoffSlider.setVisible (false);
    highFreqBoostSlider.setVisible (false);
    highFreqBoostFreqSlider.setVisible (false);
    highFreqBoostQSlider.setVisible (false);
    highFreqCutSlider.setVisible (false);
    highFreqCutFreqSlider.setVisible (false);

    if (band == EQBand::Bass)
    {
        lowFreqBoostSlider.setVisible (true);
        lowFreqCutSlider.setVisible (true);
        lowFreqCutoffSlider.setVisible (true);
    }
    else if (band == EQBand::TrebleBoost)
    {
        highFreqBoostSlider.setVisible (true);
        highFreqBoostFreqSlider.setVisible (true);
        highFreqBoostQSlider.setVisible (true);
    }
    else if (band == EQBand::TrebleCut)
    {
        highFreqCutSlider.setVisible (true);
        highFreqCutFreqSlider.setVisible (true);
    }
}

void EQChyron::resized()
{
    auto bounds = getLocalBounds().reduced (proportionOfWidth (0.025f), 0);
    bounds.removeFromTop (proportionOfHeight (labelHeightFactor));

    const auto sliderBounds = bounds.withHeight (bounds.proportionOfHeight (1.0f / 3.0f));

    const auto fourthHeight = bounds.proportionOfHeight (1.0f / 4.0f);
    lowFreqBoostSlider.setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), fourthHeight }).withY (bounds.getY() + fourthHeight / 8));
    lowFreqCutSlider.setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), 2 * fourthHeight }).withY (lowFreqBoostSlider.getBottom()));
    lowFreqCutoffSlider.setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), 3 * fourthHeight }).withY (lowFreqCutSlider.getBottom()));

    highFreqBoostSlider.setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), fourthHeight }).withY (bounds.getY() + fourthHeight / 8));
    highFreqBoostFreqSlider.setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), 2 * fourthHeight }).withY (highFreqBoostSlider.getBottom()));
    highFreqBoostQSlider.setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), 3 * fourthHeight }).withY (highFreqBoostFreqSlider.getBottom()));

    const auto thirdHeight = bounds.proportionOfHeight (1.0f / 3.0f);
    highFreqCutSlider.setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), thirdHeight }).withY (bounds.getY() + thirdHeight / 8));
    highFreqCutFreqSlider.setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), 2 * thirdHeight }).withY (highFreqCutSlider.getBottom()));
}

void EQChyron::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds();

    g.setColour (juce::Colours::black.withAlpha (0.75f));
    g.fillRoundedRectangle (bounds.toFloat(), 2.5f);

    g.setColour (colours::linesColour);
    g.drawRoundedRectangle (bounds.toFloat(), 2.5f, 1.0f);

    if (band != EQBand::None)
    {
        const auto labelBounds = bounds.withHeight (proportionOfHeight (labelHeightFactor));
        const auto labelText = [this] () -> juce::String
        {
            if (band == EQBand::Bass)
                return "Bass";
            if (band == EQBand::TrebleBoost)
                return "Treble Boost";
            if (band == EQBand::TrebleCut)
                return "Treble Cut";
            return "";
        }();
        g.setFont (juce::Font { fonts->robotoBold }.withHeight (0.8f * (float) labelBounds.getHeight()));
        g.drawFittedText (labelText, labelBounds, juce::Justification::centred, 1);
    }
}
} // namespace gui::analog_eq
