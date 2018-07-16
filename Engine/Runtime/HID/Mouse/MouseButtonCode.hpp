#pragma once

namespace usagi
{
enum class MouseButtonCode
{
    LEFT,
    RIGHT,
    MIDDLE,
    UNKNOWN,

    ENUM_COUNT,
};

const char * to_string(MouseButtonCode key);
}
