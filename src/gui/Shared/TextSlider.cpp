#include "TextSlider.h"
#include "Colours.h"

namespace gui
{
struct TextSliderLNF : chowdsp::ChowLNF
{
    juce::Slider::SliderLayout getSliderLayout (juce::Slider& slider) override
    {
        auto layout = ChowLNF::getSliderLayout (slider);
        layout.sliderBounds = slider.getLocalBounds();
        layout.textBoxBounds = slider.getLocalBounds();
        return layout;
    }

    juce::Label* createSliderTextBox (juce::Slider& slider) override
    {
        auto* label = LookAndFeel_V4::createSliderTextBox (slider);
        label->setInterceptsMouseClicks (false, false);
        label->setColour (juce::Label::outlineColourId, juce::Colours::transparentBlack);
        label->setColour (juce::Label::outlineWhenEditingColourId, juce::Colours::transparentBlack);
        label->setColour (juce::Label::textColourId, juce::Colours::transparentBlack);
        label->setColour (juce::Label::ColourIds::textWhenEditingColourId, colours::linesColour);

        label->onEditorShow = [this, label]
        {
            if (auto* editor = label->getCurrentTextEditor())
            {
                editor->setBounds (label->getLocalBounds().reduced (1));
                editor->setColour (juce::CaretComponent::caretColourId, juce::Colour (0xFFC954D4));
                editor->setColour (juce::TextEditor::backgroundColourId, colours::backgroundDark);
                editor->setJustification (juce::Justification::centred);
                editor->applyFontToAllText (juce ::Font { fonts->robotoBold }.withHeight (0.75f * (float) label->getHeight()));
            }
        };

        return label;
    }

    SharedFonts fonts;
};

TextSlider::TextSlider (chowdsp::PluginState& state, chowdsp::FloatParameter* param)
    : parameter (param),
      attachment (*param, state, *this)
{
    juce::Slider::setName (param->getName (1024));
    setLookAndFeel (lnfAllocator->getLookAndFeel<TextSliderLNF>());
    setSliderStyle (Slider::RotaryVerticalDrag);
    setMouseCursor (juce::MouseCursor::StandardCursorType::DraggingHandCursor);
}

TextSlider::~TextSlider()
{
    setLookAndFeel (nullptr);
}

void TextSlider::paint (juce::Graphics& g)
{
    g.setColour (colours::linesColour);
    g.setFont (juce ::Font { fonts->robotoBold }.withHeight (0.75f * (float) getHeight()));
    g.drawFittedText (getName() + ": " + parameter->getCurrentValueAsText(), getLocalBounds(), juce::Justification::centred, 1);
}

void TextSlider::mouseDown (const juce::MouseEvent& e)
{
    if (e.mods.isPopupMenu())
    {
        // @TODO: host context menu?
        return;
    }

    juce::Slider::mouseDown (e);
}

void TextSlider::mouseUp (const juce::MouseEvent& e)
{
    juce::Slider::mouseUp (e);

    multiClicking = e.getNumberOfClicks() > 1;
    bool dontShowLabel = e.mouseWasDraggedSinceMouseDown() || e.mods.isAnyModifierKeyDown() || e.mods.isPopupMenu() || multiClicking;
    if (! dontShowLabel)
    {
        juce::Timer::callAfterDelay (270,
                                     [safeComp = Component::SafePointer (this)]
                                     {
                                         if (safeComp == nullptr)
                                             return; // this component was deleted while waiting for the timer!

                                         if (safeComp->multiClicking)
                                         {
                                             safeComp->multiClicking = false;
                                             return;
                                         }

                                         safeComp->showTextBox();
                                     });
    }
}
} // namespace gui
