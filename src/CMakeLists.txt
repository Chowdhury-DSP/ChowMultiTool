target_sources(ChowMultiTool PRIVATE
    ChowMultiTool.cpp

    state/PluginState.cpp
    state/PresetManager.cpp

    dsp/MultiToolProcessor.cpp

    dsp/PultecEQ/PultecEQWDF.cpp
    dsp/PultecEQ/PultecEQProcessor.cpp

    dsp/Waveshaper/WaveshaperProcessor.cpp
    dsp/Waveshaper/SignalSmithWaveshaper.cpp

    dsp/EQ/EQProcessor.cpp
    dsp/SignalGenerator/SignalGeneratorProcessor.cpp
    dsp/BandSplitter/BandSplitterProcessor.cpp
    dsp/Brickwall/BrickwallProcessor.cpp
    dsp/SVF/SVFProcessor.cpp

    gui/PluginEditor.cpp
    gui/Toolbar/Toolbar.cpp
    gui/EQ/EQEditor.cpp

    gui/Waveshaper/WaveshaperEditor.cpp
    gui/Waveshaper/WaveshaperPlot.cpp
    gui/Waveshaper/BottomBar.cpp

    gui/Brickwall/BrickwallEditor.cpp
    gui/Brickwall/BrickwallPlot.cpp

    gui/PultecEQ/PultecEditor.cpp
    gui/PultecEQ/PultecPlot.cpp

    gui/BandSplitter/BandSplitterEditor.cpp
    gui/BandSplitter/BandSplitterPlot.cpp
    gui/BandSplitter/SlopePicker.cpp

    gui/SignalGenerator/SignalGeneratorEditor.cpp
    gui/SignalGenerator/OscillatorController.cpp
    gui/SignalGenerator/OscillatorPlot.cpp

    gui/SVF/SVFEditor.cpp
    gui/SVF/SVFPlot.cpp

    gui/Shared/DotSlider.cpp
)

target_precompile_headers(ChowMultiTool PRIVATE pch.h)
