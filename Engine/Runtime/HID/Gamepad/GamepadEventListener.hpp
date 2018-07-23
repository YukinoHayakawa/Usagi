#pragma once

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

class GamepadEventListener
{
public:
    virtual ~GamepadEventListener() = default;

    virtual void onButtonStateChange(const GamepadButtonEvent &e)
    {
    }
};
}
