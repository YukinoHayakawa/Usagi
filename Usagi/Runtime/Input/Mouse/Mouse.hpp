#pragma once

#include <Usagi/Math/Matrix.hpp>
#include <Usagi/Runtime/EventEmitter.hpp>
#include <Usagi/Runtime/Input/Device.hpp>

#include "MouseButtonCode.hpp"
#include "MouseEventListener.hpp"

namespace usagi
{
class Mouse
    : public Device
    , public EventEmitter<MouseEventListener>
{
    bool mImmersiveMode = false;

public:
    /**
     * \brief Get the cursor position relative to the top-left corner of
     * the focused window. If no window is focused, (0, 0) is returned.
     * \return
     */
    // todo: cursor in window pos? window::screenpostowindowpos(mouse.pos)?
    virtual Vector2f cursorPositionInActiveWindow() = 0;

    /**
     * \brief Immersive mode allows continuous movement of the mouse without
     * boundaries inside the active window.
     *
     * When enabled, the mouse cursor is hidden and the cursor can freely move
     * as if on an infinitely large surface, and a continuous sequence of mouse
     * move events will be generated. Events occur when the window is not
     * active, including the event causing the activation of the window, such
     * as clicking a mouse button within the window area when a window of
     * another application is currently active, should be ignored.
     *
     * When disabled, the mouse cursor behaves as the system default, which is
     * suitable for GUI controlling. Events occur within the visible area of
     * the window should be generated like the system default.
     */
    void setImmersiveMode(bool enable);
    bool isImmersiveMode() const { return mImmersiveMode; }

    /**
     * \brief Put the mouse cursor at the center of the rendering area of the
     * focused window.
     */
    virtual void centerCursor() = 0;
    /**
    * \brief Set the visibility of the mouse cursor.
    * \param show True to show the cursor, false to hide it.
    */
    virtual void showCursor(bool show) = 0;

    virtual bool isButtonPressed(MouseButtonCode button) const = 0;

private:
    /**
    * \brief Restrict the mouse cursor within the window render area. Mouse
    * movement events will continue to generate even if the cursor touches the
    * area border as if moving on an infinitely large surface.
    */
    virtual void captureCursor() = 0;

    /**
    * \brief Allow the mouse cursor to move outwards the rendering area of the
    * window.
    */
    virtual void releaseCursor() = 0;
    virtual bool isCursorCaptured() = 0;
};
}
