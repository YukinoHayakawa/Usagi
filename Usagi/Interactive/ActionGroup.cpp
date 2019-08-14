#include "ActionGroup.hpp"

#include <cassert>

#include <Usagi/Core/Logging.hpp>

usagi::ActionGroup::ActionGroup(Element *parent, std::string name)
    : Element(parent, std::move(name))
{
}

void usagi::ActionGroup::setBinaryActionHandler(
    std::string name,
    BinaryActionHandler handler)
{
    const auto i = mBinaryActions.insert({
        std::move(name), std::move(handler)
    });
    if(!i.second)
    {
        LOG(error, "Binary action \"{}\" is already registered!", name);
        USAGI_THROW(std::runtime_error("Action name conflict."));
    }
}

void usagi::ActionGroup::bindKey(std::string action, const KeyCode code)
{
    mKeyBindings.insert_or_assign(code, std::move(action));
}

void usagi::ActionGroup::bindMouseButton(
    std::string action,
    const MouseButtonCode code)
{
    mMouseButtonBindings.insert_or_assign(code, std::move(action));
}

void usagi::ActionGroup::setAnalogAction2DHandler(
    std::string name,
    AnalogAction2DHandler handler)
{
    const auto i = mAnalogActions2D.insert({
        std::move(name), std::move(handler)
    });
    if(!i.second)
    {
        LOG(error, "2D analog action \"{}\" is already registered!", name);
        USAGI_THROW(std::runtime_error("Action name conflict."));
    }
}

void usagi::ActionGroup::bindMouseRelativeMovement(std::string name)
{
    mMouseRelativeMoveBindings.insert(std::move(name));
}

void usagi::ActionGroup::removeAllHandlers()
{
    mBinaryActions.clear();
    mAnalogActions2D.clear();
}

void usagi::ActionGroup::activate()
{
    mActive = true;
}

void usagi::ActionGroup::deactivate()
{
    mActive = false;
    removeAllHandlers();
}

void usagi::ActionGroup::performBinaryAction(
    const std::string &name,
    const bool active)
{
    // the logic is, an action might not be bound with key events,
    // but it must has a proper handler.
    const auto i = mBinaryActions.find(name);
    if(i == mBinaryActions.end())
    {
        LOG(error, "Event is bind to non-existent action.");
        USAGI_THROW(std::logic_error("Invalid binding."));
    }
    i->second(active);
}

bool usagi::ActionGroup::onMouseMove(const MousePositionEvent &e)
{
    assert(mActive);
    const auto rel = e.distance.cast<float>();
    for(auto &&b : mMouseRelativeMoveBindings)
    {
        mAnalogActions2D.find(b)->second(rel);
    }
    return !mMouseRelativeMoveBindings.empty();
}

bool usagi::ActionGroup::onKeyStateChange(const KeyEvent &e)
{
    assert(mActive);
    const auto i = mKeyBindings.find(e.key_code);
    if(i == mKeyBindings.end()) return false;
    performBinaryAction(i->second, e.pressed);
    return true;
}

bool usagi::ActionGroup::onMouseButtonStateChange(const MouseButtonEvent &e)
{
    assert(mActive);
    const auto i = mMouseButtonBindings.find(e.button);
    if(i == mMouseButtonBindings.end()) return false;
    performBinaryAction(i->second, e.pressed);
    return true;
}
