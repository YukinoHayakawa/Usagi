#pragma once

#include <set>

#include <Usagi/Engine/Runtime/HID/Keyboard/Keyboard.hpp>

#include "WindowsHeader.hpp"

namespace usagi
{
class Win32Keyboard : public Keyboard
{
    friend class Win32Window;

    std::set<KeyCode> mKeyPressed;

    static KeyCode translate(const RAWKEYBOARD *keyboard);
    void sendKeyEvent(KeyCode key, bool pressed, bool repeated);

    void processKeyboardInput(RAWINPUT *raw);
    void clearKeyPressedStates();

public:
    bool isKeyPressed(KeyCode key) override;
};
}
