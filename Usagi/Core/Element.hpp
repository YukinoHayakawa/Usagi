#pragma once

#include <algorithm>
#include <any>
#include <cassert>
#include <functional>
#include <map>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <vector>
#include <stdexcept>

#include <Usagi/Utility/Noncopyable.hpp>

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
 * Contains component and event logic.
 */
class Element : Noncopyable
{
protected:
    Element *mParent;
    using ChildrenArray = std::vector<std::shared_ptr<Element>>;
    ChildrenArray mChildren;

    std::string mName;

	using ComponentMap = std::unordered_map<
		std::type_index, std::shared_ptr<Component>
    >;
	ComponentMap mComponents;

	void insertComponent(
		const std::type_info &type,
		std::shared_ptr<Component> component
	);

	template <typename CompBaseT>
	ComponentMap::iterator getComponentIter(const bool throws = true)
	{
		auto iter = mComponents.find(typeid(CompBaseT));
        if(iter == mComponents.end() && throws)
            throw std::runtime_error("Element has no such component.");
        return iter;
	}

    ComponentMap::iterator eraseComponent(ComponentMap::iterator i);

	std::multimap<std::type_index, std::any> mEventHandlers;

	template <typename EventT, typename... Args>
	void handleEvent(EventT &e, const std::type_info &type)
	{
        // todo handling polymorphic events?
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

    void removeChildByIter(ChildrenArray::iterator iter);

public:
    Element(Element *parent, std::string name = { });
    virtual ~Element();

    // move operations change the parent links so prohibit them.
    Element(Element &&other) = delete;
    Element & operator=(Element &&other) = delete;

    std::string name() const { return mName; }
	void setName(const std::string &name) { mName = name; }
    std::string path() const;

    // Entity Hierarchy

    Element * parent() const { return mParent; }

    template <typename ElementType = Element, typename... Args>
    ElementType * addChild(Args && ...args)
    {
        // todo enforce unique child name
        static_assert(std::is_base_of_v<Element, ElementType>,
			"ElementType is not derived from Element.");
		auto c = std::make_unique<ElementType>(
			this, std::forward<Args>(args)...
		);
        assert(c);
        // ElementCreatedEvent is fired before acceptance test, so it gets
        // a change to pass the test. (todo: is this a good idea?)
        c->template sendEvent<ElementCreatedEvent>();
        if(!acceptChild(c.get()))
            throw std::logic_error("Child element was rejected by parent.");
		const auto r = c.get();
        mChildren.push_back(std::move(c));
        sendEvent<ChildElementAddedEvent>(r);
		return r;
    }

    Element * findChild(const std::string &name) const
    {
        const auto iter = std::find_if(
            childrenBegin(), childrenEnd(),
            [&](auto &&c) { return c->name() == name; }
        );
        return iter == childrenEnd() ? nullptr : iter->get();
    }

    Element * childByName(const std::string &name) const
    {
        if(const auto c = findChild(name))
            return c;
        throw std::runtime_error("No child was found with specified name.");
    }

    Element * childByIndex(const std::size_t idx) const
    {
        assert(idx < mChildren.size());
        return mChildren[idx].get();
    }

    template <typename ChildT>
    bool hasChild(ChildT *e) const
    {
        static_assert(std::is_base_of_v<Element, ChildT>);
        return std::any_of(mChildren.begin(), mChildren.end(), [e](auto &&c) {
            return c.get() == static_cast<Element*>(e);
        });
    }

    std::size_t childrenCount() const
    {
        return mChildren.size();
    }

    void removeChild(Element *child);
    void removeChild(const std::string &name);

    // Component

    /**
     * \brief Add managed component. The created component is owned by the
     * element.
     * \tparam CompT
     * \tparam Args
     * \param args
     * \return
     */
    template <typename CompT, typename... Args>
    CompT * addComponent(Args &&... args)
    {
        auto comp = std::make_shared<CompT>(std::forward<Args>(args)...);
        const auto r = comp.get();
        insertComponent(r->baseType(), std::move(comp));
        return r;
    }

    /**
     * \brief Add unmanaged component. The caller is responsible for its
     * lifetime.
     * \param component
     */
    void addComponent(Component *component);

    template <typename CompBaseT, typename CompCastT = CompBaseT>
	CompCastT * getComponent()
    {
        const auto iter = getComponentIter<CompBaseT>();
        if constexpr(std::is_same_v<CompBaseT, CompCastT>)
            return static_cast<CompCastT*>(iter->second.get());
		else
            return dynamic_cast<CompCastT*>(iter->second.get());
    }

    template <typename CompBaseT, typename CompCastT = CompBaseT>
    CompCastT * findComponent()
    {
        const auto iter = mComponents.find(typeid(CompBaseT));
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
    void sendEvent(Args &&... args)
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

    template <typename ElementT>
    auto as()
    {
        return static_cast<ElementT*>(this);
    }

    // todo clone element (prototyping)

    ChildrenArray::const_iterator childrenBegin() const
    {
        return mChildren.begin();
    }

    ChildrenArray::const_iterator childrenEnd() const
    {
        return mChildren.end();
    }

protected:
    template <typename ChildT, typename Func>
    void forEachChild(Func f)
    {
        for(auto &&c : mChildren)
        {
            f(static_cast<ChildT*>(c.get()));
        }
    }
};
}
