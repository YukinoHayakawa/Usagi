#pragma once

#include <memory>
#include <vector>

#include <eigen3/Eigen/Core>

namespace yuki
{

enum class KeyCode;

class Mouse
{
protected:
    std::vector<std::shared_ptr<class MouseEventListener>> mMouseEventListeners;

public:
    virtual ~Mouse() = default;

    void addMouseEventListener(std::shared_ptr<MouseEventListener> listener);

    virtual Eigen::Vector2f getMouseCursorWindowPos() = 0;

    /**
     * \brief Restrict the mouse cursor within the window render area. Mouse movement
     * events will continue to generate even if the cursor touches the area border as
     * if moving on an infinitely large surface.
     */
    virtual void captureCursor() = 0;
    /**
     * \brief Allow the mouse cursor to move outwards the rendering area of the window.
     */
    virtual void releaseCursor() = 0;
    virtual bool isCursorCaptured() = 0;

    /**
     * \brief Put the mouse cursor at the center of the rendering area of the window.
     */
    virtual void centerCursor() = 0;
    /**
     * \brief Set the visibility of the mouse cursor.
     * \param show True to show the cursor, false to hide it.
     */
    virtual void showCursor(bool show) = 0;
};

}
