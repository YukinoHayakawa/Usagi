#pragma once

#include <Usagi/Core/Math.hpp>

#include "MouseButtonCode.hpp"

namespace usagi
{
struct MouseEvent
{
    class Mouse *mouse = nullptr;
    class Window *window = nullptr;
    Vector2f position;
};

struct MousePositionEvent : MouseEvent
{
    Vector2f distance;
};

struct MouseButtonEvent : MouseEvent
{
    MouseButtonCode button;
    bool pressed;
};

struct MouseWheelEvent : MouseEvent
{
    Vector2f distance;
};

class MouseEventListener
{
public:
    virtual ~MouseEventListener() = default;

    /**
     * \brief
     * \param e
     * \return true if the event is handled by the listener. false if it is not.
     */
    virtual bool onMouseMove(const MousePositionEvent &e)
    {
        return false;
    }

    /**
     * \brief
     * \param e
     * \return true if the event is handled by the listener. false if it is not.
     */
    virtual bool onMouseButtonStateChange(const MouseButtonEvent &e)
    {
        return false;
    }

    /**
     * \brief
     * \param e
     * \return true if the event is handled by the listener. false if it is not.
     */
    virtual bool onMouseWheelScroll(const MouseWheelEvent &e)
    {
        return false;
    }
};
}
