#pragma once

#include <Usagi/Core/Event/Event.hpp>

namespace usagi
{
class ChildElementEvent : public Event
{
public:
    Element * const child;

    explicit ChildElementEvent(Element *child)
        : child { child }
    {
    }
};
}
