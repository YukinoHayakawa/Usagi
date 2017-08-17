#pragma once

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
    bool mWindowActive = false;
    Eigen::Vector2i mWindowSize;

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

    static Eigen::Vector2i _parseMousePos(LPARAM lParam);
    void _sendButtonEvent(MouseButtonCode button, bool pressed);

    // keyboard

    static KeyCode _translateKeyCodeFromMessage(WPARAM vkey, LPARAM lParam);
    static int _translateKeyCodeToNative(KeyCode key);
    void _sendKeyEvent(WPARAM wParam, LPARAM lParam, bool pressed, bool repeated);
    void _confineCursorInClientArea() const;
    void _processMouseInput(const RAWINPUT *raw);

    // GetKeyState does not distinguish between them, so we manually record their states
    bool mEnterPressed = false, mNumEnterPressed = false;

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
    void captureCursor() override;
    void releaseCursor() override;
    bool isCursorCaptured() override;
    void centerCursor() override;
    void showCursor(bool show) override;

    // keyboard

    bool isKeyPressed(KeyCode key) override;
};

}
