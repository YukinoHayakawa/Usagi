#pragma once

#include <set>

#include <Usagi/Engine/Runtime/Window/Window.hpp>
#include <Usagi/Engine/Runtime/HID/Mouse/Mouse.hpp>
#include <Usagi/Engine/Runtime/HID/Keyboard/Keyboard.hpp>
#include "WindowsHeader.hpp"

namespace yuki
{

enum class MouseButtonCode;

class Win32Window
    : public Window
    , public Mouse
    , public Keyboard
{
    static const wchar_t mWindowClassName[];
    static HINSTANCE mProcessInstanceHandle;

    // window

    HWND mWindowHandle = nullptr;
    Eigen::Vector2i mWindowSize;
    bool mWindowActive;

    static void _ensureWindowSubsystemInitialized();
    RECT _getClientScreenRect() const;

    void _createWindowHandle(const std::string &title, int width, int height);
    void _registerRawInputDevices() const;

    static LRESULT CALLBACK _windowMessageDispatcher(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT _handleWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    // raw input

    std::unique_ptr<BYTE[]> _getRawInputBuffer(LPARAM lParam) const;

    // mouse

    bool mMouseCursorCaptured = false;
    /**
     * \brief Mouse cursor use a counter to determine whether it should be displayed.
     * see https://msdn.microsoft.com/en-us/library/windows/desktop/ms648396(v=vs.85).aspx
     */
    bool mShowMouseCursor = true;
    bool mMouseButtonDown[static_cast<size_t>(MouseButtonCode::ENUM_COUNT)] = { false };

    void _sendButtonEvent(MouseButtonCode button, bool pressed);

    void _captureCursor() override;
    void _releaseCursor() override;
    bool _isCursorCaptured() override;
    void _showCursor(bool show) override;
    void _recaptureCursor();
    void _confineCursorInClientArea() const;
    void _processMouseInput(const RAWINPUT *raw);

    // keyboard

    std::set<KeyCode> mKeyPressed;

    static KeyCode _translateKeyCodeFromRawInput(const RAWKEYBOARD *keyboard);
    void _sendKeyEvent(KeyCode key, bool pressed, bool repeated);

    void _processKeyboardInput(RAWINPUT *raw);
    void _clearKeyPressedStates();

public:
    /**
     * \brief Create an empty window.
     * \param title 
     * \param width 
     * \param height 
     */
    Win32Window(const std::string &title, int width, int height);

    // window

    HDC getDeviceContext() const;

    void showWindow(bool show) override;
    void processEvents() override;

    // mouse

    Eigen::Vector2f getMouseCursorWindowPos() override;
    void centerCursor() override;

    // keyboard

    bool isKeyPressed(KeyCode key) override;
};

}
