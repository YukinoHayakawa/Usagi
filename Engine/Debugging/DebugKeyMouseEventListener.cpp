#include <iostream>

#include <Usagi/Engine/Runtime/HID/Keyboard/KeyCode.hpp>

#include "DebugKeyMouseEventListener.hpp"

void yuki::DebugKeyMouseEventListener::onKeyStateChange(const KeyEvent &e)
{
    std::cout << getKeyName(e.keyCode) << (e.pressed ? " pressed" : " released") << std::endl;
}

void yuki::DebugKeyMouseEventListener::onMouseMove(const yuki::MousePositionEvent &e)
{
    std::cout << "mouse pos " << e.cursorWindowPos.transpose() << " delta " << e.cursorPosDelta.transpose() << std::endl;
}

void yuki::DebugKeyMouseEventListener::onMouseButtonStateChange(const yuki::MouseButtonEvent &e)
{
    std::cout << getMouseButtonName(e.button) << (e.pressed ? " pressed" : " released") << std::endl;
}

void yuki::DebugKeyMouseEventListener::onMouseWheelScroll(const yuki::MouseWheelEvent &e)
{
    std::cout << "scroll " << e.distance << std::endl;
}
