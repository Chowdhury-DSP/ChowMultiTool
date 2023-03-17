#include "SlopePicker.h"
#include "gui/Shared/Colours.h"

namespace gui::band_splitter
{
SlopePicker::SlopePicker (State& pluginState)
    : state (pluginState),
      params (*pluginState.params.bandSplitParams)
{
}

juce::Rectangle<float> SlopePicker::getParamIndexBounds (dsp::band_splitter::Slope slope) const noexcept
{
    static constexpr auto numSlopeChoices = (int) magic_enum::enum_count<dsp::band_splitter::Slope>();
    const auto sectionWidth = (float) getWidth() / (float) numSlopeChoices;
    return { (float) *magic_enum::enum_index (slope) * sectionWidth, 0.0f, sectionWidth, (float) getHeight() };
}

void SlopePicker::paint (juce::Graphics& g)
{
    const auto verticalGrad = [this] (juce::Colour top, juce::Colour bottom)
    {
        return juce::ColourGradient::vertical (top, 0.0f, bottom, (float) getHeight());
    };

    g.setGradientFill (verticalGrad (juce::Colours::black.withAlpha (0.0f), juce::Colours::black));
    g.fillAll();

    g.setFont (juce::Font (fonts->robotoBold).withHeight (0.6f * (float) getHeight()));
    magic_enum::enum_for_each<dsp::band_splitter::Slope> (
        [&] (auto slopeTag)
        {
            const auto isSelectedChoice = params.slope->get() == slopeTag;
            g.setGradientFill (isSelectedChoice ? verticalGrad (colours::thumbColour.withAlpha (0.0f), colours::thumbColour)
                                                : verticalGrad (colours::plotColour.withAlpha (0.0f), colours::plotColour));

            const auto slopeSectionBounds = getParamIndexBounds (slopeTag);
            g.fillRect (slopeSectionBounds);

            g.setColour (colours::linesColour);
            g.drawFittedText (params.slope->choices[(int) *magic_enum::enum_index ((dsp::band_splitter::Slope) slopeTag)],
                              slopeSectionBounds.translated (0.0f, 0.1f * (float) getHeight()).toNearestInt(),
                              juce::Justification::centred,
                              1);
        });

    g.setGradientFill (verticalGrad (colours::linesColour.withAlpha (0.25f), colours::linesColour));
    for (auto pct : { 0.2f, 0.4f, 0.6f, 0.8f })
    {
        const auto xPos = (float) getWidth() * pct;
        g.drawLine (juce::Line { juce::Point { xPos, 0.0f }, juce::Point { xPos, (float) getHeight() } }, 1.0f);
    }
}

void SlopePicker::mouseDown (const juce::MouseEvent& e)
{
    magic_enum::enum_for_each<dsp::band_splitter::Slope> (
        [this, pos = e.getPosition().toFloat()] (auto slopeTag)
        {
            if (getParamIndexBounds (slopeTag).contains (pos))
                slopeAttach.setValueAsCompleteGesture ((int) *magic_enum::enum_index ((dsp::band_splitter::Slope) slopeTag), state.undoManager);
        });
}
} // namespace gui::band_splitter
