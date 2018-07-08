#include "Keyboard.hpp"

void usagi::Keyboard::addKeyEventListener(std::shared_ptr<KeyEventListener> listener)
{
    mKeyEventListeners.push_back(std::move(listener));
}
