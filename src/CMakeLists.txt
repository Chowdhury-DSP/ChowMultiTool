target_sources(ChowMultiTool PRIVATE
    ChowMultiTool.cpp

    state/PluginState.cpp
    state/PresetManager.cpp
    state/PluginRemoteControls.cpp

    dsp/MultiToolProcessor.cpp

    dsp/AnalogEQ/PultecEQWDF.cpp
    dsp/AnalogEQ/AnalogEQProcessor.cpp

    dsp/Waveshaper/WaveshaperProcessor.cpp
    dsp/Waveshaper/SignalSmithWaveshaper.cpp
    dsp/Waveshaper/SplineWaveshaper.cpp

    dsp/EQ/EQOptimiser.cpp
    dsp/EQ/EQProcessor.cpp
    dsp/SignalGenerator/SignalGeneratorProcessor.cpp
    dsp/BandSplitter/BandSplitterProcessor.cpp
    dsp/Brickwall/BrickwallProcessor.cpp
    dsp/SVF/SVFProcessor.cpp

    gui/Shared/SpectrumAnalyserTask.cpp

    gui/PluginEditor.cpp
    gui/InitPage/InitPage.cpp

    gui/Toolbar/Toolbar.cpp
    gui/Toolbar/PresetsComponent.cpp
    gui/Toolbar/SettingsButton.cpp

    gui/EQ/EQEditor.cpp
    gui/EQ/EQPlot.cpp
    gui/EQ/BottomBar.cpp
    gui/EQ/EQBandBox.cpp
    gui/EQ/EQChyron.cpp
    gui/EQ/EQDrawView.cpp

    gui/Waveshaper/WaveshaperEditor.cpp
    gui/Waveshaper/WaveshaperPlot.cpp
    gui/Waveshaper/WaveshaperDrawer.cpp
    gui/Waveshaper/WaveshaperMathView.cpp
    gui/Waveshaper/WaveshaperPointsView.cpp
    gui/Waveshaper/WaveshaperChyron.cpp
    gui/Waveshaper/BottomBar.cpp
    gui/Waveshaper/FoldFuzzControls.cpp

    gui/Brickwall/BrickwallEditor.cpp
    gui/Brickwall/BrickwallPlot.cpp
    gui/Brickwall/BrickwallChyron.cpp
    gui/Brickwall/BottomBar.cpp

    gui/AnalogEQ/AnalogEQEditor.cpp
    gui/AnalogEQ/AnalogEQPlot.cpp
    gui/AnalogEQ/EQChyron.cpp

    gui/BandSplitter/BandSplitterEditor.cpp
    gui/BandSplitter/BandSplitterPlot.cpp

    gui/SignalGenerator/SignalGeneratorEditor.cpp
    gui/SignalGenerator/OscillatorController.cpp
    gui/SignalGenerator/OscillatorPlot.cpp
    gui/SignalGenerator/SignalGenChyron.cpp

    gui/SVF/SVFEditor.cpp
    gui/SVF/SVFPlot.cpp
    gui/SVF/SVFParamControls.cpp
    gui/SVF/SVFChyron.cpp
    gui/SVF/BottomBar.cpp

    gui/Shared/DotSlider.cpp
    gui/Shared/Fonts.cpp
    gui/Shared/SimpleBox.cpp
    gui/Shared/ErrorMessageView.cpp
    gui/Shared/TextSlider.cpp
    gui/Shared/LookAndFeels.cpp
    gui/Shared/VerticalSlider.cpp
    gui/Shared/SpectrumAnalyser.cpp
)

target_precompile_headers(ChowMultiTool PRIVATE pch.h)
