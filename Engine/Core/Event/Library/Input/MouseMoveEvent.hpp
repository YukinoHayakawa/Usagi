#pragma once

#include <Usagi/Engine/Core/Event/Event.hpp>
#include <Usagi/Engine/Core/Math.hpp>

namespace usagi
{
struct MouseMoveEvent : Event
{
    const Vector2f delta;

    explicit MouseMoveEvent(Vector2f delta)
        : delta { std::move(delta) }
    {
    }
};
}
