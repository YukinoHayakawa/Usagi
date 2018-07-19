#pragma once

#include <Usagi/Engine/Core/Math.hpp>

namespace usagi
{
struct WindowEvent
{
    class Window *window = nullptr;
};

struct WindowFocusEvent : WindowEvent
{
    bool focused;
};

struct WindowPositionEvent : WindowEvent
{
    Vector2i position;
};

struct WindowSizeEvent : WindowEvent
{
    Vector2u32 size;
};

class WindowEventListener
{
public:
    virtual ~WindowEventListener() = default;

    /**
     * \brief Fired when the window is activated/deactivated.
     * \param e 
     */
    virtual void onWindowFocusChanged(const WindowFocusEvent &e)
    {
    }

    /**
     * \brief Fired when the window begun to move. If window.setPosition()
     * is called, the listener will receive a sequence of events of MoveBegin,
     * Moved, and MoveEnd.
     * \param e 
     */
    virtual void onWindowMoveBegin(const WindowPositionEvent &e)
    {
    }

    virtual void onWindowMoved(const WindowPositionEvent &e)
    {
    }

    virtual void onWindowMoveEnd(const WindowPositionEvent &e)
    {
    }

    /**
     * \brief Fired when the window begun to resize. If window.setSize()
     * is called, the listener will receive a sequence of events of ResizeBegin,
     * Resized, and ResizeEnd.
     * \param e 
     */
    virtual void onWindowResizeBegin(const WindowSizeEvent &e)
    {
    }

    virtual void onWindowResized(const WindowSizeEvent &e)
    {
    }

    virtual void onWindowResizeEnd(const WindowSizeEvent &e)
    {
    }
};
}
