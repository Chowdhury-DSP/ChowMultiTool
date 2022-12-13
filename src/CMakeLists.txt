target_sources(ChowMultiTool PRIVATE
    ChowMultiTool.cpp

    state/PluginState.cpp

    dsp/MultiToolProcessor.cpp
    dsp/EQ/EQProcessor.cpp
    dsp/PultecEQ/PultecEQProcessor.cpp
    dsp/SignalGenerator/SignalGeneratorProcessor.cpp
    dsp/Waveshaper/WaveshaperProcessor.cpp

    gui/PluginEditor.cpp
    gui/Toolbar/Toolbar.cpp
    gui/EQ/EQEditor.cpp
    gui/PultecEQ/PultecEditor.cpp
    gui/Waveshaper/WaveshaperEditor.cpp
)

target_precompile_headers(ChowMultiTool PRIVATE pch.h)
    