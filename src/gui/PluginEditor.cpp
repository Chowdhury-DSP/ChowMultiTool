#include "PluginEditor.h"
#include "EQ/EQEditor.h"
#include "SignalGenerator/SignalGeneratorEditor.h"
#include "Waveshaper/WaveshaperEditor.h"
#include "PultecEQ/PultecEditor.h"

namespace gui
{
PluginEditor::PluginEditor (ChowMultiTool& p)
    : juce::AudioProcessorEditor (p),
      plugin (p),
      toolbar (plugin.getState())
{
    addAndMakeVisible (toolbar);

    setSize (600, 400);

    refreshEditor();
    toolChangeCallback = plugin.getState().addParameterListener (*plugin.getState().params.toolParam, true, [this] { refreshEditor(); });
}

void PluginEditor::refreshEditor()
{
    const auto toolChoice = plugin.getState().params.toolParam->getIndex() - 1;

    if (toolChoice < 0)
    {
        editorComponent.reset();
        return;
    }

    chowdsp::ForEachInTypeList<dsp::ToolTypes>::doForEach (
        [this, toolChoice] (auto toolTypeIndex)
        {
            if ((int) toolTypeIndex != toolChoice)
                return;

            using ToolType = typename dsp::ToolTypes::template AtIndex<toolTypeIndex>;
            auto& pluginState = plugin.getState();

            if constexpr (std::is_same_v<ToolType, dsp::eq::EQProcessor>)
                editorComponent = std::make_unique<eq::EQEditor> (pluginState, pluginState.params.eqParams);
            else if constexpr (std::is_same_v<ToolType, dsp::waveshaper::WaveshaperProcessor>)
                editorComponent = std::make_unique<waveshaper::WaveshaperEditor>();
            else if constexpr (std::is_same_v<ToolType, dsp::signal_gen::SignalGeneratorProcessor>)
                editorComponent = std::make_unique<signal_gen::SignalGeneratorEditor>();
            else if constexpr (std::is_same_v<ToolType, dsp::pultec::PultecEQProcessor>)
                editorComponent = std::make_unique<pultec::PultecEditor> (pluginState, pluginState.params.pultecEQParams);
        });

    addAndMakeVisible (editorComponent.get());
    resized();
}

void PluginEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

void PluginEditor::resized()
{
    auto bounds = getLocalBounds();

    toolbar.setBounds (bounds.removeFromTop (proportionOfHeight (0.1f)));

    if (editorComponent != nullptr)
        editorComponent->setBounds (bounds);
}
} // namespace gui
