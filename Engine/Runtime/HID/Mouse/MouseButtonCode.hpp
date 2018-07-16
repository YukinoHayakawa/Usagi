#pragma once

namespace usagi
{
enum class MouseButtonCode
{
    UNKNOWN,

    LEFT,
    RIGHT,
    MIDDLE,

    // BUTTON 1-3 are alises to LEFT/MIDDLE/RIGHT

    BUTTON_4,
    BUTTON_5,

    ENUM_COUNT,
};

const char * to_string(MouseButtonCode key);
}
