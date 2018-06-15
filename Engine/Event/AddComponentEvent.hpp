#pragma once

#include "Event.hpp"

namespace yuki
{
class Component;

class AddComponentEvent : public Event
{
public:
    AddComponentEvent(Component *component)
        : component { component }
    {
    }

    Component *const component;
};
}
