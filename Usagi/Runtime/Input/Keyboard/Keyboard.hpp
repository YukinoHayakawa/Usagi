#pragma once

#include <Usagi/Runtime/EventEmitter.hpp>
#include <Usagi/Runtime/Input/Device.hpp>

#include "KeyEventListener.hpp"

namespace usagi
{
enum class KeyCode;

class Keyboard
    : public Device
    , public EventEmitter<KeyEventListener>
{
public:
    virtual bool isKeyPressed(KeyCode key) const = 0;

    template <KeyCode... Keys>
    bool isAnyKeyPressed() const
    {
        return (isKeyPressed(Keys) || ...);
    }

    bool isCtrlPressed() const
    {
        return isAnyKeyPressed<KeyCode::LEFT_CONTROL, KeyCode::RIGHT_CONTROL>();
    }

    bool isAltPressed() const
    {
        return isAnyKeyPressed<KeyCode::LEFT_ALT, KeyCode::RIGHT_ALT>();
    }

    bool isShiftPressed() const
    {
        return isAnyKeyPressed<KeyCode::LEFT_SHIFT, KeyCode::RIGHT_SHIFT>();
    }

    bool isOSPressed() const
    {
        return isAnyKeyPressed<KeyCode::LEFT_OS, KeyCode::RIGHT_OS>();
    }
};
}
