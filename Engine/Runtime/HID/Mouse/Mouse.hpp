#pragma once

#include <memory>
#include <vector>

#include <eigen3/Eigen/Core>
#include "MouseButtonCode.hpp"

namespace yuki
{
enum class KeyCode;

class Mouse
{
protected:
    std::vector<std::shared_ptr<class MouseEventListener>> mMouseEventListeners;

private:
    bool mImmersiveMode = false;

public:
    virtual ~Mouse() = default;

    void addMouseEventListener(std::shared_ptr<MouseEventListener> listener);

    virtual Eigen::Vector2f getMouseCursorWindowPos() = 0;

    /**
     * \brief Immersive mode allows continuous movement of the mouse without
     * boundaries, which is suitable for gaming controls.
     * 
     * When enabled, the mouse cursor is hidden and the cursor can freely move as
     * if on an infinitely large surface, and a continuous sequence of mouse move
     * events will be generated. Events occur when the window is not active, including
     * the event causing the activation of the window, such as clicking a mouse
     * button within the window area when a window of another application is
     * currently active, should be ignored.
     * 
     * When disabled, the mouse cursor behaves as the system default, which is
     * suitable for GUI controlling. Events occur within the visible area of the
     * window should be generated like the system default.
     */
    void setImmersiveMode(bool enable);
    bool isImmersiveMode() const;

    /**
     * \brief Put the mouse cursor at the center of the rendering area of the window.
     */
    virtual void centerCursor() = 0;
    /**
    * \brief Set the visibility of the mouse cursor.
    * \param show True to show the cursor, false to hide it.
    */
    virtual void showCursor(bool show) = 0;

    virtual bool isMouseButtonPressed(MouseButtonCode button) const = 0;

private:
    /**
    * \brief Restrict the mouse cursor within the window render area. Mouse movement
    * events will continue to generate even if the cursor touches the area border as
    * if moving on an infinitely large surface.
    */
    virtual void _captureCursor() = 0;
    /**
    * \brief Allow the mouse cursor to move outwards the rendering area of the window.
    */
    virtual void _releaseCursor() = 0;
    virtual bool _isCursorCaptured() = 0;
};
}
