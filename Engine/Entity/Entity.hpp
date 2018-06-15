#pragma once

#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <typeindex>
#include <any>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki
{
class Event;
class Component;

/**
 * \brief An entity is an agent in the game representing anything visible
 * or logical. Entities are organized in a tree in logical sense. The
 * interaction with entity and the game world is carried out by components
 * and subsystems. Inter-entity communication is performed by event system.
 */
class Entity : Noncopyable
{
public:
    explicit Entity(Entity *parent);
    ~Entity();

    // move operations change the parent links so prohibit them.
    Entity(Entity &&other) = delete;
    Entity & operator=(Entity &&other) = delete;

    // Entity Hierarchy

    Entity * addChild();

    // Component

    void addComponent(std::unique_ptr<Component> component);

    // Event Handling

    template <typename EventT>
    using EventHandler = std::function<void(EventT &)>;

    template <typename EventT>
    void fireEvent(EventT &&event)
    {   // this is the only function having access to the original type
        // info of the event.
        event.setSource(this);
        handleEvent(event, typeid(event));
    }

    template <typename EventT>
    void addEventListener(EventHandler<EventT> handler)
    {
        mEventHandlers.insert({ typeid(EventT), std::move(handler) });
    }

private:
    Entity * mParent = nullptr;
    std::vector<std::unique_ptr<Entity>> mChildren;

    std::vector<std::unique_ptr<Component>> mComponents;

    std::multimap<std::type_index, std::any> mEventHandlers;

    template <typename EventT, typename... Args>
    void handleEvent(EventT &&e, const std::type_info &type)
    {
        const auto range = mEventHandlers.equal_range(type);
        for(auto i = range.first; i != range.second && !e.canceled(); ++i)
        {
            std::any_cast<EventHandler<EventT>&>(i->second)(e);
        }
        if(!e.canceled() && e.bubbling() && mParent)
        {
            mParent->handleEvent(e, type);
        }
    }
};
}
