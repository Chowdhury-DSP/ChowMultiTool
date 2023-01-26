target_sources(ChowMultiTool PRIVATE
    ChowMultiTool.cpp

    state/PluginState.cpp

    dsp/MultiToolProcessor.cpp
    dsp/EQ/EQProcessor.cpp
    dsp/PultecEQ/PultecEQProcessor.cpp
    dsp/SignalGenerator/SignalGeneratorProcessor.cpp
    dsp/Waveshaper/WaveshaperProcessor.cpp
    dsp/BandSplitter/BandSplitterProcessor.cpp

    gui/PluginEditor.cpp
    gui/Toolbar/Toolbar.cpp
    gui/EQ/EQEditor.cpp
    gui/PultecEQ/PultecEditor.cpp
    gui/Waveshaper/WaveshaperEditor.cpp

    gui/BandSplitter/BandSplitterEditor.cpp
    gui/BandSplitter/BandSplitterPlot.cpp

    gui/SignalGenerator/SignalGeneratorEditor.cpp
    gui/SignalGenerator/OscillatorController.cpp
    gui/SignalGenerator/OscillatorPlot.cpp
)

target_precompile_headers(ChowMultiTool PRIVATE pch.h)
