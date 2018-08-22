#include "InputMap.hpp"


void usagi::InputMap::bindMouseRelativeMovement(std::string name)
{
    mMouseRelativeMoveBindings.insert(std::move(name));
}

void usagi::InputMap::onMouseMove(const MousePositionEvent &e)
{

}
