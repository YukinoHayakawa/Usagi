#pragma once

#include "KeyCode.hpp"

namespace usagi
{
class Keyboard;

struct KeyboardEvent
{
    Keyboard *keyboard = nullptr;
};

struct KeyEvent : KeyboardEvent
{
    KeyCode key_code;
    bool pressed;
    bool repeated;
};

class KeyEventListener
{
public:
    virtual ~KeyEventListener() = default;

    virtual void onKeyStateChange(const KeyEvent &e)
    {
    }
};
}
