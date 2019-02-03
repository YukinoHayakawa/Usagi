#pragma once

#include <Usagi/Runtime/EventListener.hpp>

namespace usagi
{
class Gamepad;

struct GamepadEvent
{
    Gamepad *gamepad = nullptr;
};

struct GamepadButtonEvent : GamepadEvent
{
    GamepadButtonCode code;
    bool pressed;
};

class GamepadEventListener : public EventListener<GamepadEventListener>
{
public:
    virtual void onButtonStateChange(const GamepadButtonEvent &e)
    {
    }
};
}
