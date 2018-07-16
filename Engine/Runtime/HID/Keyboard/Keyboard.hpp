#pragma once

#include <vector>

namespace usagi
{
enum class KeyCode;

class Keyboard
{
protected:
    std::vector<class KeyEventListener*> mKeyEventListeners;

public:
    virtual ~Keyboard() = default;

    void addEventListener(KeyEventListener *listener);

    virtual bool isKeyPressed(KeyCode key) = 0;
};
}
