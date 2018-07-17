#pragma once

#include <set>

#include <Usagi/Engine/Runtime/HID/Keyboard/Keyboard.hpp>
#include <Usagi/Engine/Runtime/Window/WindowEventListener.hpp>

#include "WindowsHeader.hpp"

namespace usagi
{
class Win32Keyboard
    : public Keyboard
    , public WindowEventListener
{
    friend class Win32Window;
    Win32Window *mWindow;

    std::set<KeyCode> mKeyPressed;

    static KeyCode translate(const RAWKEYBOARD *keyboard);
    void sendKeyEvent(KeyCode key, bool pressed, bool repeated);

    void processKeyboardInput(RAWINPUT *raw);
    void clearKeyPressedStates();

    void onWindowFocusChanged(const WindowFocusEvent &e) override;

public:
    explicit Win32Keyboard(Win32Window *window);
    ~Win32Keyboard();

    bool isKeyPressed(KeyCode key) override;
};
}
