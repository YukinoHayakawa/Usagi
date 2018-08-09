#pragma once

namespace usagi
{
enum class MouseButtonCode
{
    LEFT,
    RIGHT,
    MIDDLE,

    // note: BUTTON 1-3 are aliases to LEFT/MIDDLE/RIGHT in Windows

    BUTTON_4,
    BUTTON_5,

    ENUM_COUNT,
};

const char * to_string(MouseButtonCode key);
}
