#pragma once

namespace yuki
{

enum class KeyCode;

struct KeyEvent
{
    class Keyboard *keyboard = nullptr;
    KeyCode keyCode;
    bool pressed = false;
    bool repeated = false;
};

class KeyEventListener
{
public:
    virtual ~KeyEventListener() = default;

    virtual void onKeyStateChange(const KeyEvent &e) { }
};

}
