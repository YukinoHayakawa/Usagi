﻿#pragma once

#include <Usagi/Engine/Runtime/HID/EventEmitter.hpp>

#include "KeyEventListener.hpp"

namespace usagi
{
enum class KeyCode;

class Keyboard : public EventEmitter<KeyEventListener>
{
public:
    virtual bool isKeyPressed(KeyCode key) = 0;
};
}
