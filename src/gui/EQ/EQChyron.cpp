#include "EQChyron.h"
#include "EQGUIHelpers.h"
#include "gui/Shared/Colours.h"

namespace gui::eq
{
EQChyron::EQChyron (chowdsp::PluginState& pluginState,
                    chowdsp::EQ::StandardEQParameters<numBands>& eqParameters,
                    const chowdsp::HostContextProvider& hcp)
    : state (pluginState),
      eqParams (eqParameters),
      hostContextProvider (hcp)
{
    updateValues();
}

void EQChyron::setSelectedBand (int newSelectedBand)
{
    if (! selectedBand.has_value() && newSelectedBand < 0)
        return; // no band selected -> no band selected

    if (selectedBand.has_value() && newSelectedBand == *selectedBand)
    {
        const auto& activeParams = eqParams.eqParams[(size_t) *selectedBand];
        const auto newFilterType = helpers::getFilterType (activeParams.typeParam->getIndex());

        if (filterType == newFilterType)
            return;
    }

    if (newSelectedBand >= 0)
        selectedBand.emplace (newSelectedBand);
    else
        selectedBand.reset();

    updateValues();

    setVisible (selectedBand.has_value());
}

void EQChyron::updateValues()
{
    const auto reset = [this]
    {
        freqSlider.reset();
        qSlider.reset();
        gainSlider.reset();
    };

    if (! selectedBand.has_value())
    {
        reset();
        return;
    }

    auto& activeParams = eqParams.eqParams[(size_t) *selectedBand];
    if (! activeParams.onOffParam->get())
    {
        reset();
        return;
    }

    freqSlider.emplace (state, activeParams.freqParam.get(), &hostContextProvider);
    freqSlider->setName ("Cutoff");
    addAndMakeVisible (*freqSlider);

    filterType = helpers::getFilterType (activeParams.typeParam->getIndex());
    if (helpers::hasQParam (filterType))
    {
        qSlider.emplace (state, activeParams.qParam.get(), &hostContextProvider);
        qSlider->setName ("Q");
        addAndMakeVisible (*qSlider);
    }
    else
    {
        qSlider.reset();
    }

    if (helpers::hasGainParam (filterType))
    {
        gainSlider.emplace (state, activeParams.gainParam.get(), &hostContextProvider);
        gainSlider->setName ("Gain");
        addAndMakeVisible (*gainSlider);
    }
    else
    {
        gainSlider.reset();
    }

    resized();
}

void EQChyron::resized()
{
    auto bounds = getLocalBounds();
    const auto sliderBounds = bounds.withHeight (proportionOfHeight (1.0f / 3.0f));

    if (freqSlider.has_value())
    {
        if (qSlider.has_value() && gainSlider.has_value())
        {
            const auto fourthHeight = proportionOfHeight (1.0f / 4.0f);
            freqSlider->setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), fourthHeight }).reduced (proportionOfWidth (0.025f), 0));
            qSlider->setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), 2 * fourthHeight }).reduced (proportionOfWidth (0.025f), 0));
            gainSlider->setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), 3 * fourthHeight }).reduced (proportionOfWidth (0.025f), 0));
        }
        else if (qSlider.has_value())
        {
            const auto thirdHeight = proportionOfHeight (1.0f / 3.0f);
            freqSlider->setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), thirdHeight }).reduced (proportionOfWidth (0.025f), 0));
            qSlider->setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), 2 * thirdHeight }).reduced (proportionOfWidth (0.025f), 0));
        }
        else
        {
            freqSlider->setBounds (sliderBounds.withCentre (bounds.getCentre()).reduced (proportionOfWidth (0.025f), 0));
        }
    }
}

void EQChyron::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds();

    g.setColour (juce::Colours::black.withAlpha (0.75f));
    g.fillRoundedRectangle (bounds.toFloat(), 2.5f);

    g.setColour (colours::linesColour);
    g.drawRoundedRectangle (bounds.toFloat(), 2.5f, 1.0f);
}
} // namespace gui::eq
