#pragma once

#include <pch.h>

namespace gui::band_splitter::colours
{
const auto backgroundLight = juce::Colour { 0xFF211F1F };
const auto backgroundDark = juce::Colour { 0xFF131111 };

const auto linesColour = juce::Colour { 0xFFD3D3D3 };
const auto majorLinesColour = linesColour.withAlpha (0.5f);
const auto minorLinesColour = linesColour.withAlpha (0.2f);
const auto plotColour = juce::Colour { 0xFF3399BB };
const auto thumbColour = juce::Colour { 0xFFA18336 };
} // namespace gui::band_splitter::colours
