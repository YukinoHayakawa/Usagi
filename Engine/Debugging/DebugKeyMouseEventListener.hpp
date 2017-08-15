#pragma once

#include <Usagi/Engine/Runtime/HID/Mouse/MouseEventListener.hpp>
#include <Usagi/Engine/Runtime/HID/Keyboard/KeyEventListener.hpp>

namespace yuki
{

class DebugKeyMouseEventListener : public MouseEventListener, public KeyEventListener
{
public:
    void onKeyStateChange(const KeyEvent &e) override;
    void onMouseMove(const yuki::MousePositionEvent &e) override;
    void onMouseButtonStateChange(const yuki::MouseButtonEvent &e) override;
    void onMouseWheelScroll(const yuki::MouseWheelEvent &e) override;
};

}
