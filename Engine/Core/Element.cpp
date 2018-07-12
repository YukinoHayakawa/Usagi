#include "Element.hpp"

#include "Component.hpp"
#include "Event/Library/AddComponentEvent.hpp"

usagi::Element::Element(Element *parent)
    : mParent { parent }
{
}

usagi::Element::~Element()
{
    // Component header is not included in Entity header so its destruction
    // can only be done here.
}

usagi::Element * usagi::Element::addChild()
{
    auto c = std::make_unique<Element>(this);
    const auto r = c.get();
    mChildren.push_back(std::move(c));
    return r;
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
    fireEvent<AddComponentEvent>(type, p);
}
