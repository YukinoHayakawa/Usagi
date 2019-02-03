#pragma once

#include <Usagi/Runtime/EventListener.hpp>

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

class KeyEventListener : public EventListener<KeyEventListener>
{
public:
    /**
     * \brief
     * \param e
     * \return true if the event is handled by the listener. false if it is not.
     */
    virtual bool onKeyStateChange(const KeyEvent &e)
    {
        return false;
    }
};
}
