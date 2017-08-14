#pragma once

namespace yuki
{

enum class KeyCode;

class KeyEventListener
{
public:
    virtual ~KeyEventListener() = default;

    struct KeyEvent
    {
        KeyCode keyCode;
        bool pressed;
        bool repeated;
    };

    virtual void onKeyStateChange(const KeyEvent &e) { }
};

}
