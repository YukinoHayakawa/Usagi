#include "MouseButtonCode.hpp"

const char * yuki::getMouseButtonName(MouseButtonCode key)
{
    switch(key)
    {
        case MouseButtonCode::LEFT: return "MouseLeft";
        case MouseButtonCode::RIGHT: return "MouseRight";
        case MouseButtonCode::MIDDLE: return "MouseMiddle";
        default: return "MouseUnknown";
    }
}
