#pragma once

#include "gui/Shared/Fonts.h"

class ChowMultiTool;
namespace gui::init
{
class InitPage : public juce::Component
{
public:
    InitPage (const ChowMultiTool&);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    const ChowMultiTool& plugin;
    std::unique_ptr<juce::Drawable> logo;
    juce::HyperlinkButton linkButton { "DSP by ChowDSP", juce::URL { "https://chowdsp.com" } };

    SharedFonts fonts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InitPage)
};
}
