#include "Entity.hpp"

#include <Usagi/Engine/Event/Library/AddComponentEvent.hpp>

#include "Component.hpp"

usagi::Entity::Entity(Entity *parent)
    : mParent { parent }
{
}

usagi::Entity::~Entity()
{
    // Component header is not included in Entity header so its destruction
    // can only be done here.
}

usagi::Entity * usagi::Entity::addChild()
{
    auto c = std::make_unique<Entity>(this);
    const auto r = c.get();
    mChildren.push_back(std::move(c));
    return r;
}

void usagi::Entity::addComponent(std::unique_ptr<Component> component)
{
    auto &info = component->getBaseTypeInfo();
    insertComponent(info, std::move(component));
}

void usagi::Entity::insertComponent(const std::type_info &type,
    std::unique_ptr<Component> component)
{
    auto p = component.get();
    mComponents.insert({ type, std::move(component) });
    fireEvent<AddComponentEvent>(type, p);
}
