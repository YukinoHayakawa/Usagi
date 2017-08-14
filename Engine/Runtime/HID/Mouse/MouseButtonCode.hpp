#pragma once

namespace yuki
{

enum class MouseButtonCode
{
    LEFT, RIGHT, MIDDLE,
    UNKNOWN
};

const char *getMouseButtonName(MouseButtonCode key);

}
