#pragma once

#include <Usagi/Engine/Runtime/EventEmitter.hpp>
#include <Usagi/Engine/Runtime/HID/HID.hpp>

#include "KeyEventListener.hpp"

namespace usagi
{
enum class KeyCode;

class Keyboard
    : public HID
    , public EventEmitter<KeyEventListener>
{
public:
    virtual bool isKeyPressed(KeyCode key) = 0;
};
}
