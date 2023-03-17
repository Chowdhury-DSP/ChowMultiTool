#pragma once

#include <pch.h>

namespace gui::colours
{
const auto backgroundLight = juce::Colour { 0xFF211F1F };
const auto backgroundDark = juce::Colour { 0xFF131111 };

const auto linesColour = juce::Colour { 0xFFD3D3D3 };
const auto majorLinesColour = linesColour.withAlpha (0.5f);
const auto minorLinesColour = linesColour.withAlpha (0.2f);
} // namespace gui::colours

namespace gui::band_splitter::colours
{
using namespace gui::colours;
const auto plotColour = juce::Colour { 0xFF3399BB };
const auto thumbColour = juce::Colour { 0xFFA18336 };
} // namespace gui::band_splitter::colours

namespace gui::waveshaper::colours
{
using namespace gui::colours;
const auto plotColour = juce::Colour { 0xFFC70C0C };
const auto boxColour = juce::Colour { 0xFF0b7189 };
} // namespace gui::waveshaper::colours
