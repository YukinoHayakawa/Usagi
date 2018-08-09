#pragma once

#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <typeinfo>
#include <typeindex>
#include <any>
#include <type_traits>
#include <algorithm>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

#include "Event/Library/Element/ElementCreatedEvent.hpp"
#include "Event/Library/Element/ChildElementAddedEvent.hpp"

namespace usagi
{
class Event;
class Component;

/**
 * \brief An element is an agent in the game representing anything visible
 * or logical. Entities are organized in a tree in logical sense. The
 * interaction with entity and the game world is carried out by components
 * and subsystems. Inter-entity communication is performed by event system.
 *
 * Contains component and event logics.
 */
class Element : Noncopyable
{
    Element *mParent;
    using ChildrenArray = std::vector<std::unique_ptr<Element>>;
    ChildrenArray mChildren;

    std::string mName;

	using ComponentMap = std::unordered_map<
		std::type_index, std::unique_ptr<Component>
    >;
	ComponentMap mComponents;

	void insertComponent(
		const std::type_info &type,
		std::unique_ptr<Component> component
	);

	template <typename CompBaseT>
	ComponentMap::iterator getComponentIter(bool throws = true)
	{
		auto iter = mComponents.find(typeid(CompBaseT));
        if(iter == mComponents.end() && throws)
            throw std::runtime_error("Element has no such component.");
        return iter;
	}

    template <typename CompBaseT, typename CompCastT = CompBaseT>
    CompCastT * castComponent(ComponentMap::iterator iter)
    {
        auto iter = getComponentIter<CompBaseT>();
        if constexpr(std::is_same_v<CompBaseT, CompCastT>)
            return static_cast<CompCastT*>(iter->second.get());
        else
            return dynamic_cast<CompCastT*>(iter->second.get());
    }

    ComponentMap::iterator eraseComponent(ComponentMap::iterator i);

	std::multimap<std::type_index, std::any> mEventHandlers;

	template <typename EventT, typename... Args>
	void handleEvent(EventT &e, const std::type_info &type)
	{
		const auto range = mEventHandlers.equal_range(type);
		for(auto i = range.first; i != range.second && !e.canceled(); ++i)
		{
			std::any_cast<EventHandler<EventT>&>(i->second)(e);
		}
		if(!e.canceled() && e.bubbling() && parent())
		{
            parent()->handleEvent(e, type);
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
    Element(Element *parent, std::string name = { });
    virtual ~Element();

    // move operations change the parent links so prohibit them.
    Element(Element &&other) = delete;
    Element & operator=(Element &&other) = delete;

    std::string name() const { return mName; }
	void setName(const std::string &name) { mName = name; }

    // Entity Hierarchy

    Element * parent() const { return mParent; }

    template <typename ElementType = Element, typename... Args>
    ElementType * addChild(Args && ...args)
    {
        static_assert(std::is_base_of_v<Element, ElementType>,
			"ElementType is not derived from Element.");
		auto c = std::make_unique<ElementType>(
			this, std::forward<Args>(args)...
		);
        // ElementCreatedEvent is fired before acceptance test, so it gets
        // a change to pass the test. (todo: is this a good idea?)
        c->template fireEvent<ElementCreatedEvent>();
        if(!acceptChild(c.get()))
            throw std::logic_error("Child element is rejected by parent.");
		const auto r = c.get();
        mChildren.push_back(std::move(c));
        fireEvent<ChildElementAddedEvent>(r);
		return r;
    }

    Element * findChildByName(const std::string &name) const
    {
        const auto iter = std::find_if(
            childrenBegin(), childrenEnd(),
            [&](auto &&c) { return c->name() == name; }
        );
        return iter == childrenEnd() ? nullptr : iter->get();
    }

    void removeChild(Element *child);

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
        auto iter = getComponentIter<CompBaseT>();
        if constexpr(std::is_same_v<CompBaseT, CompCastT>)
            return static_cast<CompCastT*>(iter->second.get());
		else
            return dynamic_cast<CompCastT*>(iter->second.get());
    }

    template <typename CompBaseT, typename CompCastT = CompBaseT>
    CompCastT * findComponent()
    {
        auto iter = mComponents.find(typeid(CompBaseT));
        if(iter == mComponents.end())
            return nullptr;
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
        eraseComponent(getComponentIter<CompBaseT>());
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

    template <typename ElementT>
    bool is() const
    {
        return dynamic_cast<ElementT*>(this) != nullptr;
    }

protected:
    ChildrenArray::const_iterator childrenBegin() const
    {
        return mChildren.begin();
    }

    ChildrenArray::const_iterator childrenEnd() const
    {
        return mChildren.end();
    }

    template <typename Func>
    void forEachChild(Func f)
    {
        for(auto &&c : mChildren)
        {
            f(c.get());
        }
    }
};
}
