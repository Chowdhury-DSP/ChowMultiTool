#pragma once

#include <pch.h>

/** Namespace containing version hints for plugin parameters */
namespace ParameterVersionHints
{
using namespace chowdsp::version_literals;

/** Version Hint 1.0.0 */
static constexpr auto version1_0_0 = "1.0.0"_v.getVersionHint();
} // namespace ParameterVersionHints
