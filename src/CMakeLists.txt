target_sources(ChowMultiTool PRIVATE
    ChowMultiTool.cpp

    state/PluginState.cpp

    dsp/MultiToolProcessor.cpp

    dsp/EQ/EQProcessor.cpp

    dsp/Waveshaper/WaveshaperProcessor.cpp

    gui/PluginEditor.cpp
    gui/Toolbar/Toolbar.cpp
)

target_precompile_headers(ChowMultiTool PRIVATE pch.h)
    