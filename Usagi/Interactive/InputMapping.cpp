#include "InputMapping.hpp"

#include <Usagi/Core/Logging.hpp>

void usagi::InputMapping::addBinaryAction(
    std::string name,
    BinaryActionHandler handler)
{
    const auto i = mBinaryActions.insert({
        std::move(name), std::move(handler)
    });
    if(!i.second)
    {
        LOG(error, "Binary action \"{}\" is already registered!", name);
        throw std::runtime_error("Action name conflict.");
    }
}

void usagi::InputMapping::bindKey(std::string action, const KeyCode code)
{
    mKeyBindings.insert_or_assign(code, std::move(action));
}

void usagi::InputMapping::bindMouseButton(
    std::string action,
    const MouseButtonCode code)
{
    mMouseButtonBindings.insert_or_assign(code, std::move(action));
}

void usagi::InputMapping::addAnalogAction2D(
    std::string name,
    AnalogAction2DHandler handler)
{
    const auto i = mAnalogActions2D.insert({
        std::move(name), std::move(handler)
    });
    if(!i.second)
    {
        LOG(error, "2D analog action \"{}\" is already registered!", name);
        throw std::runtime_error("Action name conflict.");
    }
}

void usagi::InputMapping::bindMouseRelativeMovement(std::string name)
{
    mMouseRelativeMoveBindings.insert(std::move(name));
}

void usagi::InputMapping::onMouseMove(const MousePositionEvent &e)
{
    const auto rel = e.distance.cast<float>();
    for(auto &&b : mMouseRelativeMoveBindings)
    {
        mAnalogActions2D.find(b)->second(rel);
    }
}

void usagi::InputMapping::performBinaryAction(
    const std::string &name,
    const bool active)
{
    const auto i = mBinaryActions.find(name);
    if(i == mBinaryActions.end())
    {
        LOG(error, "Event is bind to non-existent action.");
        throw std::logic_error("Invalid binding.");
    }
    i->second(active);
}

void usagi::InputMapping::onKeyStateChange(const KeyEvent &e)
{
    const auto i = mKeyBindings.find(e.key_code);
    if(i == mKeyBindings.end()) return;
    performBinaryAction(i->second, e.pressed);
}

void usagi::InputMapping::onMouseButtonStateChange(const MouseButtonEvent &e)
{
    const auto i = mMouseButtonBindings.find(e.button);
    if(i == mMouseButtonBindings.end()) return;
    performBinaryAction(i->second, e.pressed);
}
