#pragma once

#include <pch.h>

namespace gui::waveshaper::colours
{
const auto backgroundLight = juce::Colour { 0xFF211F1F };
const auto backgroundDark = juce::Colour { 0xFF131111 };

const auto linesColour = juce::Colour { 0xFFD3D3D3 };
const auto majorLinesColour = linesColour.withAlpha (0.5f);
const auto minorLinesColour = linesColour.withAlpha (0.2f);
const auto plotColour = juce::Colours::red;
const auto boxColour = juce::Colours::gold;
} // namespace gui::waveshaper::colours
