#include "Element.hpp"

#include <cassert>

#include <Usagi/Utility/DummyDeleter.hpp>

#include "Component.hpp"
#include "Logging.hpp"
#include "Event/Library/Component/ComponentAddedEvent.hpp"
#include "Event/Library/Component/PreComponentRemovalEvent.hpp"
#include "Event/Library/Component/PostComponentRemovalEvent.hpp"
#include "Event/Library/Element/PreElementRemovalEvent.hpp"
#include "Event/Library/Element/ChildElementRemovedEvent.hpp"

usagi::Element::Element(Element * const parent, std::string name)
    : mParent(parent)
    , mName(std::move(name))
{
    // name should not contain slash
    // todo this may conflict with virtual path containing slashed like the case in asset manager
    assert(name.find('/') == std::string::npos);

    LOG(info, "Created element: {}", path());
}

usagi::Element::~Element()
{
    // Component header is not included in Entity header so its destruction
    // can only be done here.

    // todo: need someway to notify the subsystems that an element has been
    // destroyed. maybe listen on removeChild?
    // Erase all components and notify the subsystems
    //for(auto i = mComponents.begin(); i != mComponents.end();
    //    i = eraseComponent(i));
}

std::string usagi::Element::path() const
{
    return mParent ? mParent->path() + '/' + name() : name();
}

void usagi::Element::removeChildByIter(ChildrenArray::iterator iter)
{
    if(iter == mChildren.end())
        throw std::runtime_error("Child element not found.");
    auto p = iter->get();
    LOG(info, "Removing element: {}", p->path());
    p->sendEvent<PreElementRemovalEvent>();
    mChildren.erase(iter);
    sendEvent<ChildElementRemovedEvent>();
}

void usagi::Element::removeChild(Element *child)
{
    const auto iter = std::find_if(
        mChildren.begin(), mChildren.end(),
        [=](auto &&c) { return c.get() == child; }
    );
    removeChildByIter(iter);
}

void usagi::Element::removeChild(const std::string &name)
{
    const auto iter = std::find_if(
        mChildren.begin(), mChildren.end(),
        [&](auto &&c) { return c->name() == name; }
    );
    removeChildByIter(iter);
}

void usagi::Element::addComponent(Component *component)
{
    auto &info = component->baseType();
    insertComponent(info, { component, DummyDeleter() });
}

void usagi::Element::insertComponent(const std::type_info &type,
    std::shared_ptr<Component> component)
{
    auto p = component.get();
    const auto r = mComponents.insert({ type, std::move(component) });
    if(!r.second)
    {
        LOG(error, "An element can only have one instance of the same type of component.");
        throw std::runtime_error("Conflicted components.");
    }
    LOG(info, "Adding component: {}[{}]", path(), p->baseType().name());
    sendEvent<ComponentAddedEvent>(type, p);
}

usagi::Element::ComponentMap::iterator usagi::Element::eraseComponent(
    ComponentMap::iterator i)
{
    const auto comp = i->second.get();
    const auto &t = comp->baseType();
    LOG(info, "Removing component: {}[{}]", path(), t.name());
    sendEvent<PreComponentRemovalEvent>(t, comp);
    i = mComponents.erase(i);
    sendEvent<PostComponentRemovalEvent>(t);
    return i;
}
