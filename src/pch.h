#pragma once

/**
 * Pre-compiled headers for JUCE plugins
 */

// C++/STL headers here...

// JUCE modules
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>

#include <chowdsp_plugin_base/chowdsp_plugin_base.h>
#include <chowdsp_plugin_state/chowdsp_plugin_state.h>
#include <chowdsp_plugin_utils/chowdsp_plugin_utils.h>
#include <chowdsp_simd/chowdsp_simd.h>
#include <chowdsp_eq/chowdsp_eq.h>
#include <chowdsp_sources/chowdsp_sources.h>
#include <chowdsp_waveshapers/chowdsp_waveshapers.h>

// Other third-party libraries
#include "../modules/chowdsp_wdf/single_include/chowdsp_wdf/chowdsp_wdf.h"

// Any other widely used headers that don't change...
#include "state/ParameterVersionHints.h"
