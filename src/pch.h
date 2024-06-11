#pragma once

/**
 * Pre-compiled headers for JUCE plugins
 */

// C++/STL headers here...

// JUCE modules
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>

#include <chowdsp_eq/chowdsp_eq.h>
#include <chowdsp_gui/chowdsp_gui.h>
#include <chowdsp_logging/chowdsp_logging.h>
#include <chowdsp_plugin_base/chowdsp_plugin_base.h>
#include <chowdsp_plugin_state/chowdsp_plugin_state.h>
#include <chowdsp_plugin_utils/chowdsp_plugin_utils.h>
#include <chowdsp_presets_v2/chowdsp_presets_v2.h>
#include <chowdsp_simd/chowdsp_simd.h>
#include <chowdsp_sources/chowdsp_sources.h>
#include <chowdsp_visualizers/chowdsp_visualizers.h>
#include <chowdsp_waveshapers/chowdsp_waveshapers.h>

#if JUCE_MODULE_AVAILABLE_melatonin_inspector
JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wreturn-type")
#include <melatonin_inspector/melatonin_inspector.h>
JUCE_END_IGNORE_WARNINGS_GCC_LIKE
#endif

// Other third-party libraries
#include <chowdsp_wdf/chowdsp_wdf.h>

JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wzero-as-null-pointer-constant",
                                     "-Wsign-conversion",
                                     "-Wimplicit-int-conversion",
                                     "-Wimplicit-float-conversion",
                                     "-Wshadow",
                                     "-Wshorten-64-to-32")
#include <Eigen/Core>
#include <LBFGSB.h>
#include <exprtk.hpp>
JUCE_END_IGNORE_WARNINGS_GCC_LIKE

// Resources
#include <cmrc/cmrc.hpp>
CMRC_DECLARE (gui);
CMRC_DECLARE (presets);

// Any other widely used headers that don't change...
#include "state/ParameterVersionHints.h"

// constants
const juce::String settingsFilePath = "ChowdhuryDSP/ChowMultiTool/.plugin_settings.json";
