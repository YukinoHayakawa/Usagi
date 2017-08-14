#pragma once

#include <memory>

#include <Usagi/Engine/Runtime/HID/Mouse/MouseEventListener.hpp>
#include <Usagi/Engine/Runtime/HID/Keyboard/KeyEventListener.hpp>

namespace yuki
{

/**
 * \brief Utilize user inputs during update operations. The input sources
 * are set up during the construction.
 */
class InteractiveComponent
    : public MouseEventListener
    , public KeyEventListener
{
protected:
    std::shared_ptr<class Mouse> mMouse;
    std::shared_ptr<class Keyboard> mKeyboard;

public:
    InteractiveComponent(std::shared_ptr<Mouse> mouse, std::shared_ptr<Keyboard> keyboard);
};

}
