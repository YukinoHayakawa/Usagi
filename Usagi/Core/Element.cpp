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

    LOG(debug, "Creating\n"
        "    Element {}: {}", static_cast<void*>(this), path());
}

usagi::Element::~Element()
{
    // Component header is not included in Entity header so its destruction
    // can only be done here.

    // todo: need someway to notify the subsystems that an element has been
    // destroyed. maybe listen on removeChild?
    // Erase all components and notify the subsystems

    // destroy children before self so that events from children are able to
    // bubble up
    mChildren.clear();

    for(auto i = mComponents.begin(); i != mComponents.end();
        i = eraseComponent(i))
    {
    }

    LOG(debug, "Destroying\n"
        "    Element {}: {}", static_cast<void*>(this), path());
}

std::string usagi::Element::path() const
{
    return mParent ? mParent->path() + '/' + name() : name();
}

void usagi::Element::removeChildByIter(ChildrenArray::iterator iter)
{
    if(iter == mChildren.end())
        USAGI_THROW(std::runtime_error("Child element not found."));
    auto p = iter->get();
    p->sendEvent<PreElementRemovalEvent>();
    mChildren.erase(iter);
    sendEvent<ChildElementRemovedEvent>();
}

void usagi::Element::removeChild(Element *child)
{
    if(child == nullptr) return;
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
        USAGI_THROW(std::runtime_error("Conflicted components."));
    }
    LOG(debug, "Adding\n"
        "    Component {}: {} to\n"
        "    Element   {}: {}",
        static_cast<void*>(p), p->baseType().name(),
        static_cast<void*>(this), path());
    sendEvent<ComponentAddedEvent>(type, p);
}

usagi::Element::ComponentMap::iterator usagi::Element::eraseComponent(
    ComponentMap::iterator i)
{
    const auto comp = i->second.get();
    const auto &t = comp->baseType();
    LOG(debug, "Removing\n"
        "    Component {}: {} from\n"
        "    Element   {}: {}",
        static_cast<void*>(comp), comp->baseType().name(),
        static_cast<void*>(this), path());
    sendEvent<PreComponentRemovalEvent>(t, comp);
    i = mComponents.erase(i);
    sendEvent<PostComponentRemovalEvent>(t);
    return i;
}
