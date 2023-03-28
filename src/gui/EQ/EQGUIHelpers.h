#pragma once

#include <pch.h>

namespace gui::eq::helpers
{
inline chowdsp::EQ::EQPlotFilterType getFilterType (int typeIndex)
{
    using Type = chowdsp::EQ::EQPlotFilterType;
    switch (typeIndex)
    {
        case 0:
            return Type::HPF1; // 6 dB
        case 1:
            return Type::HPF2; // 12 dB
        case 2:
            return Type::HPF3; // 18 dB
        case 3:
            return Type::HPF4; // 24 dB
        case 4:
            return Type::HPF8; // 48 dB

        case 5:
            return Type::LowShelf;
        case 6:
            return Type::Bell;
        case 7:
            return Type::Notch;
        case 8:
            return Type::HighShelf;

        case 9:
            return Type::LPF1;
        case 10:
            return Type::LPF2;
        case 11:
            return Type::LPF3;
        case 12:
            return Type::LPF4;
        case 13:
            return Type::LPF8;

        default:
            return {};
    }
}

constexpr bool hasGainParam (chowdsp::EQ::EQPlotFilterType filterType)
{
    using Type = chowdsp::EQ::EQPlotFilterType;
    if (filterType == Type::LPF1 || filterType == Type::LPF2 || filterType == Type::LPF3 || filterType == Type::LPF4
        || filterType == Type::LPF8 || filterType == Type::Notch || filterType == Type::HPF1 || filterType == Type::HPF2
        || filterType == Type::HPF3 || filterType == Type::HPF4 || filterType == Type::HPF8)
        return false;
    return true;
}

constexpr bool hasQParam (chowdsp::EQ::EQPlotFilterType filterType)
{
    using Type = chowdsp::EQ::EQPlotFilterType;
    return filterType != Type::LPF1 && filterType != Type::HPF1;
}
}
