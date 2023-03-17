#pragma once

#include "gui/Shared/Colours.h"
#include "gui/Shared/Fonts.h"
#include "state/PluginState.h"

namespace gui::waveshaper
{
class BottomBar : public juce::Component
{
public:
    BottomBar (State& pluginState, dsp::waveshaper::Params& wsParams);

    void resized() override;
    void paint (juce::Graphics& g) override;

private:
    struct SimpleBox : juce::ComboBox
    {
        struct LNF : juce::LookAndFeel_V4
        {
            LNF()
            {
                setColour (juce::PopupMenu::backgroundColourId, colours::backgroundDark);
                setColour (juce::PopupMenu::textColourId, colours::linesColour);
                setColour (juce::PopupMenu::highlightedTextColourId, colours::linesColour);
                setColour (juce::PopupMenu::highlightedBackgroundColourId, colours::boxColour.withAlpha (0.75f));
            }

            juce::Font getPopupMenuFont() override
            {
                return fonts->robotoBold;
            }

            gui::SharedFonts fonts;
        };

        SimpleBox()
        {
            setLookAndFeel (lnfs->getLookAndFeel<LNF>());
        }
        ~SimpleBox() override
        {
            setLookAndFeel (nullptr);
        }

        void paint (juce::Graphics& g) override
        {
            g.setFont (juce::Font (fonts->robotoBold).withHeight (0.6f * (float) getHeight()));
            g.setColour (colours::linesColour);
            g.drawFittedText (getText(), getLocalBounds(), juce::Justification::centred, 1);
        }
        void resized() override {}

        chowdsp::SharedLNFAllocator lnfs;
        gui::SharedFonts fonts;
    };

    SimpleBox shapeMenu;
    SimpleBox oversampleMenu;

    chowdsp::ComboBoxAttachment shapeAttach;
    chowdsp::ComboBoxAttachment osAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BottomBar)
};
} // namespace gui::waveshaper
