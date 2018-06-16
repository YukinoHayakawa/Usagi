#pragma once

#include <typeinfo>

#include "../Event.hpp"

namespace yuki
{
class Component;

class AddComponentEvent : public Event
{
public:
    AddComponentEvent(const std::type_info &type, Component *component)
        : type { type }
        , component { component }
    {
    }

    const std::type_info &type;
    Component *const component;
};
}
