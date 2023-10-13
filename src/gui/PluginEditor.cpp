#include "PluginEditor.h"
#include "AnalogEQ/AnalogEQEditor.h"
#include "BandSplitter/BandSplitterEditor.h"
#include "Brickwall/BrickwallEditor.h"
#include "ChowMultiTool.h"
#include "EQ/EQEditor.h"
#include "InitPage/InitPage.h"
#include "SVF/SVFEditor.h"
#include "Shared/Colours.h"
#include "SignalGenerator/SignalGeneratorEditor.h"
#include "Waveshaper/WaveshaperEditor.h"

namespace gui
{
namespace
{
    constexpr int defaultWidth = 900;
    constexpr int defaultHeight = 600;
} // namespace

PluginEditor::PluginEditor (ChowMultiTool& p)
    : juce::AudioProcessorEditor (p),
      plugin (p),
      hostContextProvider (plugin, *this),
      paintBypassCover (plugin.getState().params.bypassParam->get()),
      toolbar (plugin, oglHelper)
{
    oglHelper.setComponent (this);

    juce::Logger::writeToLog ("Checking OpenGL availability...");
    const auto shouldUseOpenGLByDefault = oglHelper.isOpenGLAvailable();
#if CHOWDSP_OPENGL_IS_AVAILABLE
    juce::Logger::writeToLog ("OpenGL is available on this system: " + juce::String (shouldUseOpenGLByDefault ? "TRUE" : "FALSE"));
#else
    juce::Logger::writeToLog ("Plugin was built without linking to OpenGL!");
#endif
    pluginSettings->addProperties<&PluginEditor::openGLChangeCallback> ({ { openGLSettingID, shouldUseOpenGLByDefault } }, *this);
    openGLChangeCallback (openGLSettingID);

    addAndMakeVisible (toolbar);
    addChildComponent (errorMessageView);

    setResizeBehaviour();

    refreshEditor();
    toolChangeCallback = plugin.getState().addParameterListener (*plugin.getState().params.toolParam,
                                                                 chowdsp::ParameterListenerThread::MessageThread,
                                                                 [this]
                                                                 { refreshEditor(); });

    juce::LookAndFeel::setDefaultLookAndFeel (lnfAllocator->getLookAndFeel<chowdsp::ChowLNF>());

    bypassChangeCallback = plugin.getState().addParameterListener (plugin.getState().params.bypassParam,
                                                                   chowdsp::ParameterListenerThread::MessageThread,
                                                                   [this]
                                                                   {
                                                                       paintBypassCover = plugin.getState().params.bypassParam->get();
                                                                       repaint();
                                                                   });
}

PluginEditor::~PluginEditor()
{
    juce::LookAndFeel::setDefaultLookAndFeel (nullptr);
}

void PluginEditor::openGLChangeCallback (chowdsp::GlobalPluginSettings::SettingID settingID)
{
    if (settingID != openGLSettingID)
        return;

    const auto shouldUseOpenGL = pluginSettings->getProperty<bool> (openGLSettingID);
    if (shouldUseOpenGL == oglHelper.isAttached())
        return; // no change

    juce::Logger::writeToLog ("Using OpenGL: " + juce::String (shouldUseOpenGL ? "TRUE" : "FALSE"));
    shouldUseOpenGL ? oglHelper.attach() : oglHelper.detach();
}

void PluginEditor::setResizeBehaviour()
{
    constrainer.setSizeLimits (int ((float) defaultWidth * 0.5f),
                               int ((float) defaultHeight * 0.5f),
                               int ((float) defaultWidth * 3.0f),
                               int ((float) defaultHeight * 3.0f));
    setConstrainer (&constrainer);
    setResizable (true, true);

    const auto& stateSize = plugin.getState().nonParams.editorBounds.get();
    setSize (stateSize.x, stateSize.y);
}

void PluginEditor::refreshEditor()
{
    const auto toolChoice = plugin.getState().params.toolParam->getIndex() - 1;

    if (toolChoice < 0)
    {
        editorComponent = std::make_unique<init::InitPage> (plugin);
    }
    else
    {
        types_list::forEach<dsp::ToolTypes> (
            [this, toolChoice] (auto toolTypeIndex)
            {
                if ((int) toolTypeIndex != toolChoice)
                    return;

                using ToolType = typename dsp::ToolTypes::template AtIndex<toolTypeIndex>;
                auto& pluginState = plugin.getState();

                if constexpr (std::is_same_v<ToolType, dsp::eq::EQProcessor>)
                    editorComponent = std::make_unique<eq::EQEditor> (pluginState, *pluginState.params.eqParams, hostContextProvider);
                else if constexpr (std::is_same_v<ToolType, dsp::waveshaper::WaveshaperProcessor>)
                    editorComponent = std::make_unique<waveshaper::WaveshaperEditor> (pluginState, *pluginState.params.waveshaperParams, hostContextProvider);
                else if constexpr (std::is_same_v<ToolType, dsp::signal_gen::SignalGeneratorProcessor>)
                    editorComponent = std::make_unique<signal_gen::SignalGeneratorEditor> (pluginState, hostContextProvider);
                else if constexpr (std::is_same_v<ToolType, dsp::analog_eq::AnalogEQProcessor>)
                    editorComponent = std::make_unique<analog_eq::AnalogEQEditor> (pluginState, *pluginState.params.analogEQParams, hostContextProvider);
                else if constexpr (std::is_same_v<ToolType, dsp::band_splitter::BandSplitterProcessor>)
                    editorComponent = std::make_unique<band_splitter::BandSplitterEditor> (pluginState, *pluginState.params.bandSplitParams, hostContextProvider);
                else if constexpr (std::is_same_v<ToolType, dsp::brickwall::BrickwallProcessor>)
                    editorComponent = std::make_unique<brickwall::BrickwallEditor> (pluginState, *pluginState.params.brickwallParams, hostContextProvider);
                else if constexpr (std::is_same_v<ToolType, dsp::svf::SVFProcessor>)
                    editorComponent = std::make_unique<svf::SVFEditor> (pluginState, *pluginState.params.svfParams, hostContextProvider);
            });
    }

    addAndMakeVisible (editorComponent.get());
    resized();
}

void PluginEditor::paint (juce::Graphics& g)
{
    g.setGradientFill (juce::ColourGradient { colours::backgroundLight,
                                              juce::Point { 0.0f, 0.0f },
                                              colours::backgroundDark,
                                              juce::Point { (float) getWidth() * 0.35f, (float) getHeight() * 0.5f },
                                              false });
    g.fillAll();
}

void PluginEditor::paintOverChildren (juce::Graphics& g)
{
    const auto toolChoice = plugin.getState().params.toolParam->getIndex();
    if (! paintBypassCover || toolChoice == 0)
        return;

    g.setColour (colours::linesColour.withAlpha (0.25f));
    if (editorComponent != nullptr)
        g.fillRect (editorComponent->getBoundsInParent());
}

void PluginEditor::resized()
{
    auto bounds = getLocalBounds();
    plugin.getState().nonParams.editorBounds = bounds.getBottomRight();

    errorMessageView.setBounds (bounds);
    toolbar.setBounds (bounds.removeFromTop (proportionOfHeight (0.1f)));

    if (editorComponent != nullptr)
        editorComponent->setBounds (bounds);
}

void PluginEditor::mouseDoubleClick ([[maybe_unused]] const juce::MouseEvent& e)
{
#if JUCE_MODULE_AVAILABLE_melatonin_inspector
    if (e.mods.isShiftDown())
    {
        inspector.setVisible (true);
        inspector.toggle (true);
    }
#endif
}
} // namespace gui
