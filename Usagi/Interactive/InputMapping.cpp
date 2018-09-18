#include "InputMapping.hpp"

#include <Usagi/Core/Logging.hpp>

void usagi::InputMapping::addAnalogAction2D(
    std::string name,
    AnalogAction2DHandler handler)
{
    const auto i =
        mAnalogActions2D.insert({ std::move(name), std::move(handler) });
    if(!i.second)
    {
        LOG(error, "Action \"{}\" is already registered!");
        throw std::runtime_error("Action name conflicted.");
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
