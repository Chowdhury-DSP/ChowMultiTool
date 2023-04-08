target_sources(ChowMultiTool PRIVATE
    ChowMultiTool.cpp

    state/PluginState.cpp
    state/PresetManager.cpp

    dsp/MultiToolProcessor.cpp

    dsp/AnalogEQ/PultecEQWDF.cpp
    dsp/AnalogEQ/AnalogEQProcessor.cpp

    dsp/Waveshaper/WaveshaperProcessor.cpp
    dsp/Waveshaper/SignalSmithWaveshaper.cpp
    dsp/Waveshaper/SplineWaveshaper.cpp

    dsp/EQ/EQProcessor.cpp
    dsp/SignalGenerator/SignalGeneratorProcessor.cpp
    dsp/BandSplitter/BandSplitterProcessor.cpp
    dsp/Brickwall/BrickwallProcessor.cpp
    dsp/SVF/SVFProcessor.cpp

    gui/PluginEditor.cpp
    gui/Toolbar/Toolbar.cpp

    gui/EQ/EQEditor.cpp
    gui/EQ/EQPlot.cpp
    gui/EQ/BottomBar.cpp
    gui/EQ/EQBandBox.cpp
    gui/EQ/EQChyron.cpp

    gui/Waveshaper/WaveshaperEditor.cpp
    gui/Waveshaper/WaveshaperPlot.cpp
    gui/Waveshaper/WaveshaperDrawer.cpp
    gui/Waveshaper/WaveshaperMathView.cpp
    gui/Waveshaper/BottomBar.cpp
    gui/Waveshaper/FoldFuzzControls.cpp

    gui/Brickwall/BrickwallEditor.cpp
    gui/Brickwall/BrickwallPlot.cpp
    gui/Brickwall/BottomBar.cpp

    gui/AnalogEQ/AnalogEQEditor.cpp
    gui/AnalogEQ/AnalogEQPlot.cpp

    gui/BandSplitter/BandSplitterEditor.cpp
    gui/BandSplitter/BandSplitterPlot.cpp

    gui/SignalGenerator/SignalGeneratorEditor.cpp
    gui/SignalGenerator/OscillatorController.cpp
    gui/SignalGenerator/OscillatorPlot.cpp

    gui/SVF/SVFEditor.cpp
    gui/SVF/SVFPlot.cpp

    gui/Shared/DotSlider.cpp
    gui/Shared/Fonts.cpp
    gui/Shared/SimpleBox.cpp
    gui/Shared/ErrorMessageView.cpp
)

target_precompile_headers(ChowMultiTool PRIVATE pch.h)
