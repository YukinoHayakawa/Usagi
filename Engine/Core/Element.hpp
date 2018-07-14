#pragma once

#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <typeinfo>
#include <typeindex>
#include <any>
#include <type_traits>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

#include "Event/Library/Element/ElementCreatedEvent.hpp"
#include "Event/Library/Element/ChildElementAddedEvent.hpp"
#include "Event/Library/Component/PreComponentRemovalEvent.hpp"
#include "Event/Library/Component/PostComponentRemovalEvent.hpp"

namespace usagi
{
class Event;
class Component;

/**
 * \brief An element is an agent in the game representing anything visible
 * or logical. Entities are organized in a tree in logical sense. The
 * interaction with entity and the game world is carried out by components
 * and subsystems. Inter-entity communication is performed by event system.
 */
class Element : Noncopyable
{
	Element *mParent = nullptr;
	using ChildrenArray = std::vector<std::unique_ptr<Element>>;
	ChildrenArray mChildren;

	using ComponentMap = std::unordered_map<
		std::type_index, std::unique_ptr<Component>
    >;
	ComponentMap mComponents;

	void insertComponent(
		const std::type_info &type,
		std::unique_ptr<Component> component
	);

	template <typename CompBaseT>
	ComponentMap::iterator findComponent()
	{
		auto iter = mComponents.find(typeid(CompBaseT));
		if(iter == mComponents.end())
			throw std::runtime_error(
				"Entity does not have such a component.");
        return iter;
	}

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

    /**
     * \brief Invoked before adding a child. If false is returned, the addition
     * is aborted and a std::logic_error is thrown by addChild().
     * \param child 
     * \return 
     */
    virtual bool acceptChild(Element *child)
    {
        return true;
    }

public:
    explicit Element(Element *parent);
    virtual ~Element();

    // move operations change the parent links so prohibit them.
    Element(Element &&other) = delete;
    Element & operator=(Element &&other) = delete;

    // Entity Hierarchy

    template <typename ElementType = Element>
    ElementType * addChild()
    {
        static_assert(std::is_base_of_v<Element, ElementType>,
			"ElementType is not derived from Element.");
		auto c = std::make_unique<ElementType>(this);
        // ElementCreatedEvent is fired before acceptance test, so it gets
        // a change to pass the test. (todo: is this a good idea?)
        c->template fireEvent<ElementCreatedEvent>();
        if(!acceptChild(c.get()))
            throw std::logic_error("Child element cannot be added.");
		const auto r = c.get();
		mChildren.push_back(std::move(c));
        fireEvent<ChildElementAddedEvent>();
		return r;
    }

    void removeChild(Element *child);

    ChildrenArray::const_iterator childrenBegin() const
    {
        return mChildren.begin();
    }

    ChildrenArray::const_iterator childrenEnd() const
    {
        return mChildren.end();
    }

    // Component

    template <typename CompT, typename... Args>
    CompT * addComponent(Args &&... args)
    {
        auto comp = std::make_unique<CompT>(std::forward<Args>(args)...);
        const auto r = comp.get();
        insertComponent(comp->getBaseTypeInfo(), std::move(comp));
        return r;
    }

    void addComponent(std::unique_ptr<Component> component);

    template <typename CompBaseT, typename CompCastT = CompBaseT>
	CompCastT * getComponent()
    {
        auto iter = findComponent<CompBaseT>();
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

    template <typename CompBaseT>
    void removeComponent()
    {
		auto comp = findComponent<CompBaseT>()->second.get();
        fireEvent<PreComponentRemovalEvent>(comp->getBaseTypeInfo(), comp);
        mComponents.erase(comp);
        fireEvent<PostComponentRemovalEvent>(comp->getBaseTypeInfo());
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

    template <typename EventBaseT>
    void addEventListener(EventHandler<EventBaseT> handler)
    {
        mEventHandlers.insert({ typeid(EventBaseT), std::move(handler) });
    }
};
}
