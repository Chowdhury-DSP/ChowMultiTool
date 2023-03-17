#pragma once

#include <pch.h>

namespace gui
{
struct Fonts
{
    Fonts();

    juce::Typeface::Ptr robotoBold;
    //    juce::Typeface::createSystemTypefaceFor ()
};
using SharedFonts = juce::SharedResourcePointer<Fonts>;
} // namespace gui
