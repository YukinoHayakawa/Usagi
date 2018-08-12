#pragma once

#include <Usagi/Engine/Core/Math.hpp>
#include <Usagi/Engine/Runtime/EventEmitter.hpp>

#include "WindowEventListener.hpp"

namespace usagi
{
/**
* \brief Window is the interface between the player and the engine. It is mainly
* for receiving user inputs, but can also be the target of graphical rendering.
*/
class Window : public EventEmitter<WindowEventListener>
{
public:
    virtual ~Window() = default;

    virtual Vector2i position() const = 0;
    virtual void setPosition(const Vector2i &position) = 0;

    /**
     * \brief Get client area size.
     * \return
     */
    virtual Vector2u32 size() const = 0;

    /**
     * \brief Set client area size (excluding title bar, border, etc.)
     * \param size
     */
    virtual void setSize(const Vector2u32 &size) = 0;

    /**
     * \brief Get window title encoded using UTF-8.
     * \return
     */
    virtual std::string title() const = 0;

    /**
     * \brief
     * \param title New window title encoded using UTF-8.
     */
    virtual void setTitle(std::string title) = 0;

    virtual void show(bool show) = 0;
    virtual bool focused() const = 0;
    virtual bool isOpen() const = 0;
    virtual void close() = 0;

    virtual Vector2f dpiScale() const = 0;
};
}
