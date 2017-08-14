#pragma once

#include <Usagi/Engine/Runtime/Window/Window.hpp>
#include <Usagi/Engine/Runtime/HID/Mouse/Mouse.hpp>
#include <Usagi/Engine/Runtime/HID/Keyboard/Keyboard.hpp>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

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

    HWND mWindowHandle = nullptr;

    Eigen::Vector2f mMouseCursorLastPos;

    /**
    * \brief Initialize the window subsystem.
    */
    static void _ensureWindowSubsystemInitialized();

    static LRESULT CALLBACK _windowMessageDispatcher(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT _handleWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static Eigen::Vector2f _parseMousePos(LPARAM lParam);
    void _sendButtonEvent(MouseButtonCode button, bool pressed, LPARAM lParam);
    static KeyCode _translateKeyCodeFromMessage(WPARAM vkey, LPARAM lParam);
    static int _translateKeyCodeToNative(KeyCode key);
    void _sendKeyEvent(WPARAM wParam, LPARAM lParam, bool pressed, bool repeated);

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

    HDC getDeviceContext() const;

    void show() override;
    void hide() override;
    void processEvents() override;

    bool isKeyPressed(KeyCode key) override;
    Eigen::Vector2f getMouseCursorWindowPos() override;
};

}
