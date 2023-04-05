#pragma once

#include "Fonts.h"

namespace gui
{
template <typename ParamEnumType>
class ChoicePicker : public juce::Component
{
public:
    ChoicePicker (chowdsp::PluginState& pluginState, chowdsp::EnumChoiceParameter<ParamEnumType>& choiceParam)
        : state (pluginState),
          param (choiceParam)
    {
    }

    void paint (juce::Graphics& g) override
    {
        const auto verticalGrad = [this] (juce::Colour top, juce::Colour bottom)
        {
            return juce::ColourGradient::vertical (top, 0.0f, bottom, (float) getHeight());
        };

        g.setGradientFill (verticalGrad (juce::Colours::black.withAlpha (0.0f), juce::Colours::black));
        g.fillAll();

        g.setFont (juce::Font (fonts->robotoBold).withHeight (0.6f * (float) getHeight()));
        magic_enum::enum_for_each<ParamEnumType> (
            [&] (auto paramTag)
            {
                const auto isSelectedChoice = param.get() == paramTag;
                g.setGradientFill (isSelectedChoice ? verticalGrad (thumbColour.withAlpha (0.0f), thumbColour)
                                                    : verticalGrad (plotColour.withAlpha (0.0f), plotColour));

                const auto sectionBounds = getParamIndexBounds (paramTag);
                g.fillRect (sectionBounds);

                g.setColour (linesColour);
                g.drawFittedText (param.choices[(int) *magic_enum::enum_index ((ParamEnumType) paramTag)],
                                  sectionBounds.translated (0.0f, 0.1f * (float) getHeight()).toNearestInt(),
                                  juce::Justification::centred,
                                  1);
            });

        g.setGradientFill (verticalGrad (linesColour.withAlpha (0.25f), linesColour));
        const auto recipNumChoices = 1.0f / (float) magic_enum::enum_count<ParamEnumType>();
        for (auto pct = recipNumChoices; pct < 1.0f; pct += recipNumChoices)
        {
            const auto xPos = (float) getWidth() * pct;
            g.drawLine (juce::Line { juce::Point { xPos, 0.0f }, juce::Point { xPos, (float) getHeight() } }, 1.0f);
        }
    }

    juce::Colour linesColour;
    juce::Colour thumbColour;
    juce::Colour plotColour;

private:
    void mouseDown (const juce::MouseEvent& e) override
    {
        magic_enum::enum_for_each<ParamEnumType> (
            [this, pos = e.getPosition().toFloat()] (auto paramTag)
            {
                if (getParamIndexBounds (paramTag).contains (pos))
                    attach.setValueAsCompleteGesture ((int) *magic_enum::enum_index ((ParamEnumType) paramTag), state.undoManager);
            });
    }

    juce::Rectangle<float> getParamIndexBounds (ParamEnumType paramVal) const noexcept
    {
        static constexpr auto numChoices = (int) magic_enum::enum_count<ParamEnumType>();
        const auto sectionWidth = (float) getWidth() / (float) numChoices;
        return { (float) *magic_enum::enum_index (paramVal) * sectionWidth, 0.0f, sectionWidth, (float) getHeight() };
    }

    chowdsp::PluginState& state;
    chowdsp::EnumChoiceParameter<ParamEnumType>& param;
    chowdsp::ParameterAttachment<chowdsp::ChoiceParameter> attach { param, state, [this] (int)
                                                                    { repaint(); } };

    gui::SharedFonts fonts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChoicePicker)
};
} // namespace gui
