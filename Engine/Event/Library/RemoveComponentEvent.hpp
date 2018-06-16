#pragma once

#include <typeinfo>

#include "../Event.hpp"

namespace yuki
{
class Component;

class RemoveComponentEvent : public Event
{
public:
    RemoveComponentEvent(const std::type_info &type, Component *component)
        : type{ type }
        , component{ component }
    {
    }

    const std::type_info &type;
    Component *const component;
};
}
