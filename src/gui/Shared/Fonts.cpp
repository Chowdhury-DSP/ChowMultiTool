#include "Fonts.h"

namespace gui
{
Fonts::Fonts()
{
    const auto fs = cmrc::gui::get_filesystem();
    const auto robotoBoldFont = fs.open ("Fonts/Roboto-Bold.ttf");
    robotoBold = juce::Typeface::createSystemTypefaceFor (robotoBoldFont.begin(), robotoBoldFont.size());
}
}
