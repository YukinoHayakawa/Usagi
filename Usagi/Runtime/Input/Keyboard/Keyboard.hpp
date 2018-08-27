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
    virtual bool isKeyPressed(KeyCode key) = 0;
};
}
