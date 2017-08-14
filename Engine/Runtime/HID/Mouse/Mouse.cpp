#include "Mouse.hpp"

void yuki::Mouse::addMouseEventListener(std::shared_ptr<MouseEventListener> listener)
{
    mMouseEventListeners.push_back(std::move(listener));
}
