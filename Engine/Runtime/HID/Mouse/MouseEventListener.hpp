#pragma once

#include <eigen3/Eigen/Core>

#include "MouseButtonCode.hpp"

namespace yuki
{

struct MouseEvent
{
    // the origin is the upper-left corner of the window's drawing area
    Eigen::Vector2f cursorWindowPos;
};

struct MousePositionEvent : MouseEvent
{
    Eigen::Vector2f cursorPosDelta;
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

    virtual void onMouseMove(const MousePositionEvent &e) { }
    virtual void onMouseButtonStateChange(const MouseButtonEvent &e) { }
    virtual void onMouseWheelScroll(const MouseWheelEvent &e) { }
};

}
