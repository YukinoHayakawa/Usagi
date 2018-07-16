#pragma once

#include "KeyCode.hpp"

namespace usagi
{
class Keyboard;

struct KeyEvent
{
    class Keyboard *keyboard = nullptr;
    KeyCode key_code = KeyCode::UNKNOWN;
    bool pressed = false;
    bool repeated = false;
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
