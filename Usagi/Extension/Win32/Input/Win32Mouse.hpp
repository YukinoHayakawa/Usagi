#pragma once

#include <Usagi/Runtime/Input/Mouse/Mouse.hpp>
#include <Usagi/Runtime/Window/WindowEventListener.hpp>

#include "Win32RawInputDevice.hpp"

namespace usagi
{
class Win32Platform;

class Win32Mouse
    : public Mouse
    , public Win32RawInputDevice
    , public WindowEventListener
{
    friend class Win32InputManager;

    bool mMouseCursorCaptured = false;

    /**
     * \brief Mouse cursor uses a counter to determine whether it should be
     * displayed.
     * see https://msdn.microsoft.com/en-us/library/windows/desktop/ms648396(v=vs.85).aspx
     */
    bool mShowMouseCursor = true;
    bool mMouseButtonDown[static_cast<std::size_t>(MouseButtonCode::ENUM_COUNT)]
        = { false };

    void sendButtonEvent(MouseButtonCode button, bool pressed);
    void sendWheelEvent(const Vector2f &distance);

    void recaptureCursor();

    /**
     * \brief Restrict the mouse cursor inside the client area of active window.
     */
    static void confineCursorInClientArea();

    // todo remove
    void onWindowFocusChanged(const WindowFocusEvent &e) override;
    void onWindowMoveEnd(const WindowPositionEvent &e) override;
    void onWindowResizeEnd(const WindowSizeEvent &e) override;

    void handleRawInput(RAWINPUT *raw) override;

public:
    Win32Mouse(HANDLE device_handle, std::string name);

    std::string name() const override;

    Vector2f cursorPositionInActiveWindow() override;
    void centerCursor() override;
    void showCursor(bool show) override;
    bool isButtonPressed(MouseButtonCode button) const override;

private:
    void captureCursor() override;
    void releaseCursor() override;
    bool isCursorCaptured() override;
};
}
