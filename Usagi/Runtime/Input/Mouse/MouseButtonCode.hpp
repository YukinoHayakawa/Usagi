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

    // virtual buttons activated when rolling the mouse wheel.
    // since they are only pressed when wheel event occurs, immediate checks
    // will always return released state.
    // the press event will be immediately followed by a release event.

    WHEEL_UP,
    WHEEL_DOWN,

    ENUM_COUNT,
};

const char * to_string(MouseButtonCode key);
}
