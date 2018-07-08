#pragma once

namespace usagi
{
enum class MouseButtonCode
{
    LEFT,
    RIGHT,
    MIDDLE,
    ENUM_COUNT
};

const char * getMouseButtonName(MouseButtonCode key);
}
