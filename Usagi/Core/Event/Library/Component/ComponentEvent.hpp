#pragma once

#include <typeinfo>

#include <Usagi/Core/Event/Event.hpp>

namespace usagi
{
class Component;

class ComponentEvent : public Event
{
public:
    ComponentEvent(const std::type_info &type, Component *component)
        : type { type }
        , component { component }
    {
    }

    const std::type_info &type;
    Component *const component;
};
}
