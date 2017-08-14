#include "Keyboard.hpp"

void yuki::Keyboard::addKeyEventListener(std::shared_ptr<KeyEventListener> listener)
{
    mKeyEventListeners.push_back(std::move(listener));
}
