#pragma once

#include <Usagi/Engine/Core/Math.hpp>

namespace usagi
{
/**
* \brief Window is the interface between the player and the engine. It is mainly
* for receiving user inputs, but can also be the target of graphical rendering.
*/
class Window
{
public:
    virtual ~Window() = default;

    virtual void show(bool show) = 0;
    virtual bool isFocused() const = 0;
    virtual bool isOpen() const = 0;
    virtual Vector2f size() const = 0;
    virtual void setTitle(const std::string &title) = 0;

    virtual void processEvents() = 0;
};
}
