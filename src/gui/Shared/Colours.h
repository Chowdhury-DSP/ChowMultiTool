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
const auto thumbColour = juce::Colour { 0xFFF0A202 };
} // namespace gui::band_splitter::colours

namespace gui::waveshaper::colours
{
using namespace gui::colours;
const auto plotColour = juce::Colour { 0xFFC70C0C };
const auto boxColour = juce::Colour { 0xFF0b7189 };
} // namespace gui::waveshaper::colours

namespace gui::brickwall::colours
{
using namespace gui::colours;
const auto plotColour = juce::Colour { 0xFFC03221 };
const auto thumbColour = juce::Colour { 0xFF4B8F8C };
} // namespace gui::brickwall::colours

namespace gui::eq::colours
{
using namespace gui::colours;
const auto plotColour = juce::Colour { 0xFFC03221 };
const auto thumbColour = juce::Colour { 0xFF4B8F8C };
} // namespace gui::eq::colours
