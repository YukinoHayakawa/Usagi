#pragma once

#include <Usagi/Math/Matrix.hpp>
#include <Usagi/Runtime/EventListener.hpp>

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
    bool sequence;
};

struct WindowSizeEvent : WindowEvent
{
    Vector2u32 size;
    bool sequence;
};

struct WindowCharEvent : WindowEvent
{
    char utf8[4] = { };
    std::uint32_t utf32 = 0;
    std::uint16_t utf16 = 0;
};

class WindowEventListener : public EventListener<WindowEventListener>
{
public:
    /**
     * \brief Fired when the window is activated/deactivated.
     * \param e
     */
    virtual void onWindowFocusChanged(const WindowFocusEvent &e)
    {
    }

    /**
     * \brief Fired when the user begin to move a window. Between
     * onWindowMoveBegin and onWindowMoveEnd events there may be zero or more
     * onWindowMoved events depending on the actual window movement.
     * e.sequence will be set to true in these events.
     * \param e
     */
    virtual void onWindowMoveBegin(const WindowPositionEvent &e)
    {
    }

    /**
     * \brief If triggered by window.setPosition or maximize, a sequence of
     * onWindowMoveBegin, onWindowMoved, and onWindowMoveEnd
     * events will be triggered with e.sequence set to false.
     * \param e
     */
    virtual void onWindowMoved(const WindowPositionEvent &e)
    {
    }

    virtual void onWindowMoveEnd(const WindowPositionEvent &e)
    {
    }

    /**
     * \brief Similar to onWindowMoveBegin.
     * \param e
     */
    virtual void onWindowResizeBegin(const WindowSizeEvent &e)
    {
    }

    /**
     * \brief Similar to onWindowMoved
     * \param e
     */
    virtual void onWindowResized(const WindowSizeEvent &e)
    {
    }

    virtual void onWindowResizeEnd(const WindowSizeEvent &e)
    {
    }

    virtual void onWindowMinimized(const WindowSizeEvent &e)
    {
    }

    virtual void onWindowRestored(const WindowSizeEvent &e)
    {
    }

    virtual void onWindowCharInput(const WindowCharEvent &e)
    {
    }
};
}
