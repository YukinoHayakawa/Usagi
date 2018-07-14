#pragma once

#include <Usagi/Engine/Core/Event/Event.hpp>

namespace usagi
{
class ChildElementEvent : Event
{
public:
	Element * const child;

    explicit ChildElementEvent(Element *child)
        : child { child }
    {
    }
};
}
