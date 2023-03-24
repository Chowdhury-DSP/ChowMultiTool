#include "EQPlot.h"
#include "gui/Shared/Colours.h"
#include "gui/Shared/FrequencyPlotHelpers.h"

namespace gui::eq
{
chowdsp::EQ::EQPlotFilterType getFilterType (int typeIndex)
{
    using Type = chowdsp::EQ::EQPlotFilterType;
    switch (typeIndex)
    {
        case 0:
            return Type::HPF1; // 6 dB
        case 1:
            return Type::HPF2; // 12 dB
        case 2:
            return Type::HPF3; // 18 dB
        case 3:
            return Type::HPF4; // 24 dB
        case 4:
            return Type::HPF8; // 48 dB

        case 5:
            return Type::LowShelf;
        case 6:
            return Type::Bell;
        case 7:
            return Type::Notch;
        case 8:
            return Type::HighShelf;

        case 9:
            return Type::LPF1;
        case 10:
            return Type::LPF2;
        case 11:
            return Type::LPF3;
        case 12:
            return Type::LPF4;
        case 13:
            return Type::LPF8;

        default:
            return {};
    }
}

constexpr bool hasGainParam (chowdsp::EQ::EQPlotFilterType filterType)
{
    using Type = chowdsp::EQ::EQPlotFilterType;
    if (filterType == Type::LPF1 || filterType == Type::LPF2 || filterType == Type::LPF3 || filterType == Type::LPF4
        || filterType == Type::LPF8 || filterType == Type::Notch || filterType == Type::HPF1 || filterType == Type::HPF2
        || filterType == Type::HPF3 || filterType == Type::HPF4 || filterType == Type::HPF8)
        return false;
    return true;
}

constexpr bool hasQParam (chowdsp::EQ::EQPlotFilterType filterType)
{
    using Type = chowdsp::EQ::EQPlotFilterType;
    return filterType != Type::LPF1 && filterType != Type::HPF1;
}

juce::Rectangle<float> EQPlot::QDotSlider::getThumbBounds() const noexcept
{
    const auto dim = plotBase.getLocalBounds().proportionOfWidth (0.025f);
    return juce::Rectangle { dim, dim }
        .toFloat()
        .withCentre (juce::Point { getXCoordinate(), getYCoordinate() });
}

const auto qRange = chowdsp::ParamUtils::createNormalisableRange (0.5f, 20.0f, chowdsp::CoefficientCalculators::butterworthQ<float>);

double EQPlot::QDotSlider::proportionOfLengthToValue (double proportion)
{
    const auto ret = (double) qRange.convertFrom0to1 ((float) proportion);
    return ret;
}

double EQPlot::QDotSlider::valueToProportionOfLength (double value)
{
    return (double) qRange.convertTo0to1 ((float) value);
}

constexpr int minFrequency = 18;
constexpr int maxFrequency = 22'000;

EQPlot::EQPlot (chowdsp::PluginState& pluginState, chowdsp::EQ::StandardEQParameters<numBands>& eqParameters)
    : chowdsp::EQ::EqualizerPlotWithParameters<numBands> (pluginState.getParameterListeners(),
                                                          eqParameters,
                                                          &getFilterType,
                                                          chowdsp::SpectrumPlotParams {
                                                              .minFrequencyHz = minFrequency,
                                                              .maxFrequencyHz = maxFrequency })
{
    for (size_t i = 0; i < numBands; ++i)
    {
        freqSliders[i].emplace (*eqParameters.eqParams[i].freqParam,
                                pluginState,
                                *this,
                                SpectrumDotSlider::Orientation::FrequencyOriented);
        addChildComponent (*freqSliders[i]);
        freqSliders[i]->getYCoordinate = [this, i, &eqParameters]
        {
            return getYCoordinateForDecibels (hasGainParam (getFilterType (eqParameters.eqParams[i].typeParam->getIndex()))
                                                  ? eqParameters.eqParams[i].gainParam->get()
                                                  : 0.0f);
        };
        freqSliders[i]->setVisible (eqParameters.eqParams[i].onOffParam->get());

        gainSliders[i].emplace (*eqParameters.eqParams[i].gainParam,
                                pluginState,
                                *this,
                                SpectrumDotSlider::Orientation::MagnitudeOriented);
        addChildComponent (*gainSliders[i]);
        gainSliders[i]->getXCoordinate = [this, i, &eqParameters]
        {
            return getXCoordinateForFrequency (eqParameters.eqParams[i].freqParam->get());
        };
        gainSliders[i]->setVisible (eqParameters.eqParams[i].onOffParam->get() && hasGainParam (getFilterType (eqParameters.eqParams[i].typeParam->getIndex())));

        qSliders[i].emplace (*eqParameters.eqParams[i].qParam,
                             pluginState,
                             *this,
                             SpectrumDotSlider::Orientation::MagnitudeOriented);
        qSliders[i]->checkModifierKeys = [] (const juce::ModifierKeys& mods)
        { return mods.isCommandDown(); };
        qSliders[i]->setVelocityModeParameters (1.0, 1, 0.0, true, juce::ModifierKeys::altModifier);
        addChildComponent (*qSliders[i]);
        qSliders[i]->getYCoordinate = [this, i, &eqParameters]
        {
            return getYCoordinateForDecibels (hasGainParam (getFilterType (eqParameters.eqParams[i].typeParam->getIndex()))
                                                  ? eqParameters.eqParams[i].gainParam->get()
                                                  : 0.0f);
        };
        qSliders[i]->getXCoordinate = [this, i, &eqParameters]
        {
            return getXCoordinateForFrequency (eqParameters.eqParams[i].freqParam->get());
        };
        qSliders[i]->setVisible (eqParameters.eqParams[i].onOffParam->get() && hasQParam (getFilterType (eqParameters.eqParams[i].typeParam->getIndex())));

        callbacks += {
            pluginState.addParameterListener (*eqParameters.eqParams[i].onOffParam,
                                              chowdsp::ParameterListenerThread::MessageThread,
                                              [this, i, &eqParameters]
                                              {
                                                  freqSliders[i]->setVisible (eqParameters.eqParams[i].onOffParam->get());
                                                  gainSliders[i]->setVisible (eqParameters.eqParams[i].onOffParam->get() && hasGainParam (getFilterType (eqParameters.eqParams[i].typeParam->getIndex())));
                                                  qSliders[i]->setVisible (eqParameters.eqParams[i].onOffParam->get() && hasQParam (getFilterType (eqParameters.eqParams[i].typeParam->getIndex())));
                                                  repaint();
                                              }),
            pluginState.addParameterListener (*eqParameters.eqParams[i].typeParam,
                                              chowdsp::ParameterListenerThread::MessageThread,
                                              [this, i, &eqParameters]
                                              {
                                                  gainSliders[i]->setVisible (eqParameters.eqParams[i].onOffParam->get() && hasGainParam (getFilterType (eqParameters.eqParams[i].typeParam->getIndex())));
                                                  qSliders[i]->setVisible (eqParameters.eqParams[i].onOffParam->get() && hasQParam (getFilterType (eqParameters.eqParams[i].typeParam->getIndex())));
                                                  repaint();
                                              }),
        };

        sliderGroups[i].setSliders ({ &(*freqSliders[i]), &(*gainSliders[i]), &(*qSliders[i]) });
        addAndMakeVisible (sliderGroups[i]);
    }
}

void EQPlot::paint (juce::Graphics& g)
{
    gui::drawFrequencyLines<minFrequency, maxFrequency> (*this,
                                                         g,
                                                         { 100.0f, 1'000.0f, 10'000.0f },
                                                         colours::majorLinesColour,
                                                         colours::minorLinesColour);
    gui::drawMagnitudeLines (*this,
                             g,
                             { -20.0f, -10.0f, 0.0f, 10.0f, 20.0f },
                             { 0.0f },
                             colours::majorLinesColour,
                             colours::minorLinesColour);

    g.setColour (juce::Colours::yellow);
    g.strokePath (getMasterFilterPath(), juce::PathStrokeType { 2.5f });
}

void EQPlot::resized()
{
    EqualizerPlotWithParameters::resized();
    for (auto& group : sliderGroups)
        group.setBounds (getLocalBounds());
}
} // namespace gui::eq
