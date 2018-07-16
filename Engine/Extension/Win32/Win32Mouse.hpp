﻿#pragma once

#include <Usagi/Engine/Runtime/HID/Mouse/Mouse.hpp>

#include "WindowsHeader.hpp"

namespace usagi
{
class Win32Window;

class Win32Mouse : public Mouse
{
    friend class Win32Window;
    Win32Window *mWindow = nullptr;

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
    void confineCursorInClientArea() const;
    void processMouseInput(const RAWINPUT *raw);

public:
    explicit Win32Mouse(Win32Window *window);

    Vector2f getCursorPositionInWindow() override;
    void centerCursor() override;
    void showCursor(bool show) override;
    bool isButtonPressed(MouseButtonCode button) const override;

private:
    void captureCursor() override;
    void releaseCursor() override;
    bool isCursorCaptured() override;
};
}
