#include "Element.hpp"

#include "Component.hpp"
#include "Event/Library/Component/ComponentAddedEvent.hpp"

usagi::Element::Element(std::string name)
    : mName { std::move(name) }
{
}

usagi::Element::~Element()
{
    // Component header is not included in Entity header so its destruction
    // can only be done here.
}

void usagi::Element::addComponent(std::unique_ptr<Component> component)
{
    auto &info = component->getBaseTypeInfo();
    insertComponent(info, std::move(component));
}

void usagi::Element::insertComponent(const std::type_info &type,
    std::unique_ptr<Component> component)
{
    auto p = component.get();
    mComponents.insert({ type, std::move(component) });
    fireEvent<ComponentAddedEvent>(type, p);
}
