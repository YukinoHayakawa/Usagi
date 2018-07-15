#include "Element.hpp"

#include <algorithm>

#include "Component.hpp"
#include "Event/Library/Component/ComponentAddedEvent.hpp"
#include "Event/Library/Element/PreElementRemovalEvent.hpp"
#include "Event/Library/Element/ChildElementRemovedEvent.hpp"

usagi::Element::Element(Element *parent, std::string name)
    : mParent { parent }
    , mName { std::move(name) }
{
}

usagi::Element::~Element()
{
    // Component header is not included in Entity header so its destruction
    // can only be done here.
}

void usagi::Element::removeChild(Element *child)
{
    const auto iter = std::find_if(
		mChildren.begin(), mChildren.end(),
		[=](auto &&c) { return c.get() == child; }
	);
    if(iter == mChildren.end())
        throw std::runtime_error("Cannot find specified child.");
    auto p = iter->get();
    p->fireEvent<PreElementRemovalEvent>();
    mChildren.erase(iter);
    fireEvent<ChildElementRemovedEvent>();
}

usagi::Element * usagi::Element::findChildByName(const std::string &name) const
{
    const auto iter = std::find_if(
		childrenBegin(), childrenEnd(),
		[&](auto &&c) { return c->name() == name; }
	);
    return iter == childrenEnd() ? nullptr : iter->get();
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
