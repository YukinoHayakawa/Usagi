#include "Entity.hpp"

#include <Usagi/Engine/Event/AddComponentEvent.hpp>

#include "Component.hpp"

yuki::Entity::Entity(Entity *parent)
    : mParent { parent }
{
}

yuki::Entity::~Entity()
{
    // Component header is not included in Entity header so its destruction
    // can only be done here.
}

yuki::Entity * yuki::Entity::addChild()
{
    auto c = std::make_unique<Entity>(this);
    const auto r = c.get();
    mChildren.push_back(std::move(c));
    return r;
}

void yuki::Entity::appendComponent(std::unique_ptr<Component> component)
{
    auto p = component.get();
    mComponents.push_back(std::move(component));
    fireEvent<AddComponentEvent>(p);
}
