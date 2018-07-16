#include "Keyboard.hpp"

void usagi::Keyboard::addEventListener(KeyEventListener *listener)
{
    mKeyEventListeners.push_back(listener);
}
