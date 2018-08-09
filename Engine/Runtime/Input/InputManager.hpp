#pragma once

#include <memory>
#include <vector>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace usagi
{
class Keyboard;
class Mouse;
class Gamepad;

class InputManager : Noncopyable
{
public:
    virtual ~InputManager() = default;

    /**
     * \brief Read input events from system event queue and dispatch
     * them to corresponding devices.
     */
    virtual void processEvents() = 0;

    // todo: multiple keyboards/mice?
    /**
     * \brief A virtual device receiving messages from any keyboard.
     * \return
     */
    virtual std::shared_ptr<Keyboard> virtualKeyboard() const = 0;

    /**
     * \brief A virtual device receiving messages from any mouse.
     * \return
     */
    virtual std::shared_ptr<Mouse> virtualMouse() const = 0;

    virtual std::vector<std::shared_ptr<Gamepad>> gamepads() const = 0;
};
}
