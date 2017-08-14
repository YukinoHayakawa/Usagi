#pragma once

#include <memory>
#include <vector>

namespace yuki
{

enum class KeyCode;

class Keyboard

{
protected:
    std::vector<std::shared_ptr<class KeyEventListener>> mKeyEventListeners;

public:
    virtual ~Keyboard() = default;

    void addKeyEventListener(std::shared_ptr<KeyEventListener> listener);

    virtual bool isKeyPressed(KeyCode key) = 0;
};

}
