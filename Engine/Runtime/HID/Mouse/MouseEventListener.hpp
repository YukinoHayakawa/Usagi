#pragma once

#include <Usagi/Engine/Core/Math.hpp>

#include "MouseButtonCode.hpp"

namespace usagi
{
struct MouseEvent
{
    class Mouse *mouse = nullptr;
};

struct MousePositionEvent : MouseEvent
{
    Vector2f cursor_position_delta = Vector2f::Zero();
};

struct MouseButtonEvent : MouseEvent
{
    MouseButtonCode button = MouseButtonCode::UNKNOWN;
    bool pressed = false;
};

struct MouseWheelEvent : MouseEvent
{
    int distance = 0;
};

class MouseEventListener
{
public:
    virtual ~MouseEventListener() = default;

    virtual void onMouseMove(const MousePositionEvent &e)
    {
    }

    virtual void onMouseButtonStateChange(const MouseButtonEvent &e)
    {
    }

    virtual void onMouseWheelScroll(const MouseWheelEvent &e)
    {
    }
};
}
