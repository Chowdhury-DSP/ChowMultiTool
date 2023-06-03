#include "ErrorMessageView.h"
#include "gui/PluginEditor.h"

namespace gui
{
ErrorMessageView::ErrorMessageView()
{
    addAndMakeVisible (title);
    title.setJustificationType (juce::Justification::centred);
    title.setColour (juce::Label::ColourIds::textColourId, juce::Colours::red);

    addAndMakeVisible (message);
    message.setJustificationType (juce::Justification::centred);

    addAndMakeVisible (closeButton);
    closeButton.onClick = [this]
    { setVisible (false); };

    addAndMakeVisible (yesButton);
    yesButton.onClick = [this]
    {
        setVisible (false);
    };

    addAndMakeVisible (noButton);
    noButton.onClick = [this]
    {
        setVisible (false);
    };
}

PluginEditor* findTopLevelEditor (juce::Component* currentComponent)
{
    PluginEditor* topLevelEditor = nullptr;
    while (currentComponent != nullptr)
    {
        topLevelEditor = dynamic_cast<PluginEditor*> (currentComponent);
        if (topLevelEditor != nullptr)
            break;
        currentComponent = currentComponent->getParentComponent();
    }

    // Unable to find top level component!
    jassert (topLevelEditor != nullptr);

    return topLevelEditor;
}

void ErrorMessageView::showErrorMessage (const juce::String& title, const juce::String& message, const juce::String& buttonText, Component* comp)
{
    // similar to:
    //    NativeMessageBox::showAsync (MessageBoxOptions()
    //                                     .withIconType (MessageBoxIconType::WarningIcon)
    //                                     .withTitle (title)
    //                                     .withMessage (message)
    //                                     .withButton (buttonText)
    //                                     .withAssociatedComponent (comp),
    //                                 nullptr);

    if (auto* topLevelEditor = findTopLevelEditor (comp))
    {
        auto& errorMessageView = topLevelEditor->getErrorMessageView();
        errorMessageView.setParameters (title, message, buttonText);
        errorMessageView.setVisible (true);
    }
}

void ErrorMessageView::showYesNoBox (const juce::String& title,
                                     const juce::String& message,
                                     juce::Component* comp,
                                     const std::function<void (bool)>& onChoice)
{
    // similar to:
    //    return NativeMessageBox::showYesNoBox (MessageBoxIconType::WarningIcon, title, message, comp) == 1;

    if (auto* topLevelEditor = findTopLevelEditor (comp))
    {
        auto& errorMessageView = topLevelEditor->getErrorMessageView();
        errorMessageView.setParametersYesNo (title, message, onChoice);
        errorMessageView.setVisible (true);
    }
}

void ErrorMessageView::setParameters (const juce::String& titleText,
                                      const juce::String& messageText,
                                      const juce::String& buttonText)
{
    setAlwaysOnTop (true);
    title.setText (titleText, juce::dontSendNotification);
    message.setText (messageText, juce::dontSendNotification);
    closeButton.setButtonText (buttonText);

    closeButton.setVisible (true);
    yesButton.setVisible (false);
    noButton.setVisible (false);
}

void ErrorMessageView::setParametersYesNo (const juce::String& titleText,
                                           const juce::String& messageText,
                                           const std::function<void (bool)>& onChoice)
{
    setAlwaysOnTop (true);
    title.setText (titleText, juce::dontSendNotification);
    message.setText (messageText, juce::dontSendNotification);

    closeButton.setVisible (false);
    yesButton.setVisible (true);
    noButton.setVisible (true);

    yesButton.onClick = [this, onChoice]
    {
        setVisible (false);
        onChoice (true);
    };
    noButton.onClick = [this, onChoice]
    {
        setVisible (false);
        onChoice (false);
    };
}

void ErrorMessageView::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black.withAlpha (0.85f));
}

void ErrorMessageView::resized()
{
    const auto titleX = proportionOfWidth (0.1f);
    const auto titleWidth = proportionOfWidth (0.8f);
    const auto titleY = proportionOfHeight (0.05f);
    const auto titleHeight = proportionOfHeight (0.1f);
    title.setBounds (titleX, titleY, titleWidth, titleHeight);
    title.setFont (juce::Font { (float) titleHeight * 0.8f }.boldened());

    const auto messageX = proportionOfWidth (0.1f);
    const auto messageWidth = proportionOfWidth (0.8f);
    const auto messageY = proportionOfHeight (0.2f);
    const auto messageHeight = proportionOfHeight (0.5f);
    message.setBounds (messageX, messageY, messageWidth, messageHeight);
    message.setFont (juce::Font { (float) messageHeight * 0.075f });

    const auto buttonX = proportionOfWidth (0.45f);
    const auto buttonWidth = proportionOfWidth (0.1f);
    const auto buttonY = proportionOfHeight (0.8f);
    const auto buttonHeight = proportionOfHeight (0.1f);
    closeButton.setBounds (buttonX, buttonY, buttonWidth, buttonHeight);

    const auto yesX = proportionOfWidth (0.375f);
    yesButton.setBounds (yesX, buttonY, buttonWidth, buttonHeight);

    const auto noX = proportionOfWidth (0.525f);
    noButton.setBounds (noX, buttonY, buttonWidth, buttonHeight);
}
} // namespace gui
