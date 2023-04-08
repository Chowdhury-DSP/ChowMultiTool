#pragma once

#include <pch.h>

namespace gui
{
#pragma once

#include <pch.h>

class ErrorMessageView : public juce::Component
{
public:
    ErrorMessageView();

    static void showErrorMessage (const juce::String& title, const juce::String& message, const juce::String& buttonText, juce::Component* comp);

    /** Returns true if the answer is yes */
    static bool showYesNoBox (const juce::String& title, const juce::String& message, Component* comp);

    void setParameters (const juce::String& title, const juce::String& message, const juce::String& buttonText);
    void setParametersYesNo (const juce::String& title, const juce::String& message);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    juce::Label title;
    juce::Label message;
    juce::TextButton closeButton;
    juce::TextButton yesButton { "YES" };
    juce::TextButton noButton { "NO" };
    int result = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ErrorMessageView)
};
}
