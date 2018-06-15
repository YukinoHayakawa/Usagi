#pragma once

#include "Event.hpp"

namespace yuki
{
class Component;

class RemoveComponentEvent : public Event
{
public:
    RemoveComponentEvent(Component *component)
        : component { component }
    {
    }

    Component *const component;
};
}
