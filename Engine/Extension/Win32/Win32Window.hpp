#pragma once

#include <set>

#include <Usagi/Engine/Runtime/Window/Window.hpp>
#include <Usagi/Engine/Runtime/HID/Mouse/Mouse.hpp>
#include <Usagi/Engine/Runtime/HID/Keyboard/Keyboard.hpp>

#include "WindowsHeader.hpp"

namespace usagi
{
enum class MouseButtonCode;

class Win32Window
    : public Window
    , public Mouse
    , public Keyboard
{
    static const wchar_t WINDOW_CLASS_NAME[];
    static HINSTANCE mProcessInstanceHandle;

    // window

    HWND mWindowHandle = nullptr;
    Vector2u32 mWindowSize;
    bool mWindowActive = false;
    bool mClosed = false;

    static void ensureWindowSubsystemInitialized();
    RECT getClientScreenRect() const;

    void createWindowHandle(const std::string &title, int width, int height);
    void _registerRawInputDevices() const;

    static LRESULT CALLBACK windowMessageDispatcher(HWND hWnd, UINT message,
        WPARAM wParam, LPARAM lParam);
    LRESULT handleWindowMessage(HWND hWnd, UINT message,
        WPARAM wParam,LPARAM lParam);

    // raw input

    std::unique_ptr<BYTE[]> getRawInputBuffer(LPARAM lParam) const;

    // mouse

    bool mMouseCursorCaptured = false;
    /**
     * \brief Mouse cursor use a counter to determine whether it should be
     * displayed.
     * see https://msdn.microsoft.com/en-us/library/windows/desktop/ms648396(v=vs.85).aspx
     */
    bool mShowMouseCursor = true;
    bool mMouseButtonDown[static_cast<std::size_t>(MouseButtonCode::ENUM_COUNT)]
        = { false };

    void sendButtonEvent(MouseButtonCode button, bool pressed);

    void captureCursor() override;
    void releaseCursor() override;
    bool isCursorCaptured() override;
    void showCursor(bool show) override;
    void recaptureCursor();
    void confineCursorInClientArea() const;
    void processMouseInput(const RAWINPUT *raw);

    // keyboard

    std::set<KeyCode> mKeyPressed;

    static KeyCode translate(const RAWKEYBOARD *keyboard);
    void sendKeyEvent(KeyCode key, bool pressed, bool repeated);

    void processKeyboardInput(RAWINPUT *raw);
    void clearKeyPressedStates();

public:
    /**
     * \brief Create an empty window.
     * \param title 
     * \param size 
     */
    Win32Window(const std::string &title, const Vector2u32 &size);

    // window

    HDC getDeviceContext() const;
    HWND getNativeWindowHandle() const;
    static HINSTANCE getProcessInstanceHandle();

    void show(bool show) override;
    bool isFocused() const override;
    Vector2f size() const override;
    bool isOpen() const override;
    void setTitle(const std::string &title) override;

    void processEvents() override;

    // mouse

    Vector2f getCursorPositionInWindow() override;
    void centerCursor() override;
    bool isButtonPressed(MouseButtonCode button) const override;

    // keyboard

    bool isKeyPressed(KeyCode key) override;
};
}
