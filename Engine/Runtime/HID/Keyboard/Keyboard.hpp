#pragma once

#include <Usagi/Engine/Runtime/EventEmitter.hpp>
#include <Usagi/Engine/Runtime/HID/Device.hpp>

#include "KeyEventListener.hpp"

namespace usagi
{
enum class KeyCode;

class Keyboard
    : public Device
    , public EventEmitter<KeyEventListener>
{
public:
    virtual bool isKeyPressed(KeyCode key) = 0;
};
}
