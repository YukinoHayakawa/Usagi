#pragma once

#include <set>

#include <Usagi/Runtime/Input/Keyboard/Keyboard.hpp>
#include <Usagi/Runtime/Window/WindowEventListener.hpp>

#include "Win32RawInputDevice.hpp"

namespace usagi
{
class Win32Keyboard
    : public Keyboard
    , public Win32RawInputDevice
    , public WindowEventListener
{
    friend class Win32InputManager;

    std::set<KeyCode> mKeyPressed;

    void handleRawInput(RAWINPUT *data) override;
    static KeyCode translate(const RAWKEYBOARD *keyboard);
    void sendKeyEvent(KeyCode key, bool pressed, bool repeated);

    void clearKeyPressedStates();

    // todo remove
    void onWindowFocusChanged(const WindowFocusEvent &e) override;

public:
    Win32Keyboard(HANDLE device_handle, std::string name);

    std::string name() const override;

    bool isKeyPressed(KeyCode key) override;
};
}
