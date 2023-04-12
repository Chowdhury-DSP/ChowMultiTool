#include "EQChyron.h"
#include "EQGUIHelpers.h"
#include "gui/Shared/Colours.h"

namespace gui::eq
{
EQChyron::EQChyron (chowdsp::PluginState& pluginState, chowdsp::EQ::StandardEQParameters<numBands>& eqParameters)
    : state (pluginState),
      eqParams (eqParameters)
{
    updateValues();
}

void EQChyron::setSelectedBand (int newSelectedBand)
{
    setVisible (newSelectedBand >= 0);
    if (selectedBand == newSelectedBand)
        return;

    selectedBand = newSelectedBand;
    updateValues();
}

void EQChyron::updateValues()
{
    const auto reset = [this]
    {
        freqSlider.reset();
        qSlider.reset();
        gainSlider.reset();
    };

    if (selectedBand < 0)
    {
        reset();
        return;
    }

    auto& activeParams = eqParams.eqParams[(size_t) selectedBand];
    if (! activeParams.onOffParam->get())
    {
        reset();
        return;
    }

    freqSlider.emplace (state, activeParams.freqParam.get());
    freqSlider->setName ("Cutoff");
    addAndMakeVisible (*freqSlider);

    const auto filterType = helpers::getFilterType (activeParams.typeParam->getIndex());
    if (helpers::hasQParam (filterType))
    {
        qSlider.emplace (state, activeParams.qParam.get());
        qSlider->setName ("Q");
        addAndMakeVisible (*qSlider);
    }
    else
    {
        qSlider.reset();
    }

    if (helpers::hasGainParam (filterType))
    {
        gainSlider.emplace (state, activeParams.gainParam.get());
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
            freqSlider->setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), fourthHeight }));
            qSlider->setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), 2 * fourthHeight }));
            gainSlider->setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), 3 * fourthHeight }));
        }
        else if (qSlider.has_value())
        {
            const auto thirdHeight = proportionOfHeight (1.0f / 3.0f);
            freqSlider->setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), thirdHeight }));
            qSlider->setBounds (sliderBounds.withCentre ({ bounds.getCentreX(), 2 * thirdHeight }));
        }
        else
        {
            freqSlider->setBounds (sliderBounds.withCentre (bounds.getCentre()));
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
