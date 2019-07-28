#pragma once

#include <Usagi/Math/Matrix.hpp>
#include <Usagi/Runtime/EventListener.hpp>

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
    /**
     * \brief Determining the direction:
     * Imagine a track ball that can roll in two directions and it moves the
     * mouse cursor. It is more intuitive that rolling up moves the cursor up
     * on the screen and rolling left moves it left. Now most mice only have
     * a wheel that is capable of rolling in one direction and we call the
     * directions up and down. Since our convention is to use right-handed
     * coordinate systems, moving up and right corresponds to positive
     * directions, therefore we use those as positive directions in this
     * variable.
     */
    Vector2f distance;
};

class MouseEventListener : public EventListener<MouseEventListener>
{
public:
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
