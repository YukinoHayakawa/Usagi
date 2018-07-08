#pragma once

#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <typeinfo>
#include <typeindex>
#include <any>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace usagi
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

    template <typename CompT, typename... Args>
    CompT * addComponent(Args &&... args)
    {
        auto comp = std::make_unique<CompT>(std::forward<Args>(args)...);
        const auto r = comp.get();
        insertComponent(typeid(std::decay_t<CompT>), std::move(comp));
        return r;
    }

    void addComponent(std::unique_ptr<Component> component);

    template <typename CompBaseT, typename CompCastT = CompBaseT>
	CompCastT * getComponent()
    {
        auto iter = mComponents.find(typeid(CompBaseT));
        if(iter == mComponents.end())
            throw std::runtime_error(
                "Entity does not have such component");
        if constexpr(std::is_same_v<CompBaseT, CompCastT>)
            return static_cast<CompCastT*>(iter->second.get());
		else
            return dynamic_cast<CompCastT*>(iter->second.get());
    }

    template <typename CompT>
    bool hasComponent()
    {
        // todo: c++20, use contains()
        return mComponents.find(typeid(CompT)) != mComponents.end();
    }

    // Event Handling

    template <typename EventT>
    using EventHandler = std::function<void(EventT &)>;

    template <typename EventT, typename... Args>
    void fireEvent(Args &&... args)
    {
        EventT event { std::forward<Args>(args)... };
        event.setSource(this);
        handleEvent(event, typeid(event));
    }

    template <typename EventT>
    void addEventListener(EventHandler<EventT> handler)
    {
        mEventHandlers.insert({ typeid(EventT), std::move(handler) });
    }

private:
    Entity *mParent = nullptr;
    std::vector<std::unique_ptr<Entity>> mChildren;

    std::unordered_map<
        std::type_index,
        std::unique_ptr<Component>
    > mComponents;

    void insertComponent(
        const std::type_info &type,
        std::unique_ptr<Component> component
    );

    std::multimap<std::type_index, std::any> mEventHandlers;

    template <typename EventT, typename... Args>
    void handleEvent(EventT &e, const std::type_info &type)
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
