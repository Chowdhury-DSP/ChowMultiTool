#include "EQChyron.h"
#include "EQGUIHelpers.h"
#include "gui/Shared/Colours.h"

namespace gui::eq
{
EQChyron::EQChyron (chowdsp::PluginState& pluginState, chowdsp::EQ::StandardEQParameters<numBands>& eqParameters)
    : eqParams (eqParameters)
{
    for (size_t i = 0; i < numBands; ++i)
    {
        callbacks += {
            pluginState.addParameterListener (eqParameters.eqParams[i].onOffParam,
                                              chowdsp::ParameterListenerThread::MessageThread,
                                              [this]
                                              { updateValues(); }),
            pluginState.addParameterListener (eqParameters.eqParams[i].typeParam,
                                              chowdsp::ParameterListenerThread::MessageThread,
                                              [this]
                                              { updateValues(); }),
            pluginState.addParameterListener (eqParameters.eqParams[i].freqParam,
                                              chowdsp::ParameterListenerThread::MessageThread,
                                              [this]
                                              { updateValues(); }),
            pluginState.addParameterListener (eqParameters.eqParams[i].qParam,
                                              chowdsp::ParameterListenerThread::MessageThread,
                                              [this]
                                              { updateValues(); }),
            pluginState.addParameterListener (eqParameters.eqParams[i].gainParam,
                                              chowdsp::ParameterListenerThread::MessageThread,
                                              [this]
                                              { updateValues(); }),
        };
    }

    updateValues();
}

void EQChyron::setSelectedBand (int newSelectedBand)
{
    selectedBand = newSelectedBand;
    setVisible (selectedBand >= 0);
    updateValues();
}

void EQChyron::updateValues()
{
    const auto reset = [this]
    {
        freqValue.reset();
        qValue.reset();
        gainValue.reset();
    };

    if (selectedBand < 0)
    {
        reset();
        return;
    }

    const auto& activeParams = eqParams.eqParams[(size_t) selectedBand];
    if (activeParams.onOffParam->get() == false)
    {
        reset();
        return;
    }

    freqValue.emplace (activeParams.freqParam->getCurrentValueAsText());

    const auto filterType = helpers::getFilterType (activeParams.typeParam->getIndex());
    if (helpers::hasQParam (filterType))
        qValue.emplace (activeParams.qParam->getCurrentValueAsText());
    else
        qValue.reset();

    if (helpers::hasGainParam (filterType))
        gainValue.emplace (activeParams.gainParam->getCurrentValueAsText());
    else
        gainValue.reset();
}

void EQChyron::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    g.setColour (juce::Colours::black.withAlpha (0.75f));
    g.fillRoundedRectangle (bounds.toFloat(), 2.5f);

    g.setColour (colours::linesColour);
    g.drawRoundedRectangle (bounds.toFloat(), 2.5f, 1.0f);

    g.setFont (juce ::Font { fonts->robotoBold }.withHeight (0.25f * (float) getHeight()));
    if (freqValue.has_value())
    {
        if (qValue.has_value() && gainValue.has_value())
        {
            const auto thirdHeight = proportionOfHeight (1.0f / 3.0f);
            const auto cutoffBounds = bounds.removeFromTop (thirdHeight).reduced (2, 0);
            g.drawFittedText ("Cutoff: " + *freqValue, cutoffBounds, juce::Justification::centred, 1);

            const auto qBounds = bounds.removeFromTop (thirdHeight).reduced (2, 0);
            g.drawFittedText ("Q: " + *qValue, qBounds, juce::Justification::centred, 1);

            const auto gainBounds = bounds.removeFromTop (thirdHeight).reduced (2, 0);
            g.drawFittedText ("Gain: " + *gainValue, gainBounds, juce::Justification::centred, 1);
        }
        else if (qValue.has_value())
        {
            const auto halfHeight = proportionOfHeight (0.5f);
            const auto cutoffBounds = bounds.removeFromTop (halfHeight).reduced (2, 0);
            g.drawFittedText ("Cutoff: " + *freqValue, cutoffBounds, juce::Justification::centred, 1);

            const auto qBounds = bounds.removeFromTop (halfHeight).reduced (2, 0);
            g.drawFittedText ("Q: " + *qValue, qBounds, juce::Justification::centred, 1);
        }
        else
        {
            const auto cutoffBounds = bounds.reduced (2, 0);
            g.drawFittedText ("Cutoff: " + *freqValue, cutoffBounds, juce::Justification::centred, 1);
        }
    }
}
} // namespace gui::eq
