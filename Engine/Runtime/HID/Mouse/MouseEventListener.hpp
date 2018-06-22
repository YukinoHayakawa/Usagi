#pragma once

#include <eigen3/Eigen/Core>

#include "MouseButtonCode.hpp"

namespace yuki
{
struct MouseEvent
{
    class Mouse *mouse = nullptr;
};

struct MousePositionEvent : MouseEvent
{
    Eigen::Vector2f cursorPosDelta = Eigen::Vector2f::Zero();
};

struct MouseButtonEvent : MouseEvent
{
    MouseButtonCode button;
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
