#pragma once

/** Namespace containing version hints for plugin parameters */
namespace ParameterVersionHints
{
// Version hints are handled as follows:
// Assume the plugin version has the form MAJOR.MINOR.PATCH,
// then the plugin version hint = 10,000 * MAJOR + 100 * MINOR + PATCH.
// This way we can handle up to 99 major and minor versions
// without breaking the system.
// Patch versions are not included in this paradigm, since
// making a change as big as adding a new parameter should
// require at least a minor release.

/** Version Hint 1.0.0 */
static constexpr int version1_0_0 = 10'000;
}
