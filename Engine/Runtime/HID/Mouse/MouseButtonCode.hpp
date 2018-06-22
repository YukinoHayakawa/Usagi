#pragma once

namespace yuki
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
