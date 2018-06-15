#include "Entity.hpp"

#include "Component.hpp"

yuki::Entity::Entity(Entity *parent)
    : mParent { parent }
{
}

yuki::Entity::~Entity()
{
}

yuki::Entity * yuki::Entity::addChild()
{
    auto c = std::make_unique<Entity>(this);
    const auto r = c.get();
    mChildren.push_back(std::move(c));
    return r;
}

void yuki::Entity::addComponent(std::unique_ptr<Component> component)
{
    mComponents.push_back(std::move(component));
}
