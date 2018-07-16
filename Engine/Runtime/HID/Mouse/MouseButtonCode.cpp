#include "MouseButtonCode.hpp"

const char * usagi::to_string(const MouseButtonCode key)
{
    switch(key)
    {
        case MouseButtonCode::LEFT: return "MouseLeft";
        case MouseButtonCode::RIGHT: return "MouseRight";
        case MouseButtonCode::MIDDLE: return "MouseMiddle";
        case MouseButtonCode::BUTTON_4: return "Mouse4";
        case MouseButtonCode::BUTTON_5: return "Mouse5";
        default: return "MouseUnknown";
    }
}
