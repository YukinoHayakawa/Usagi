#pragma once

#include <Usagi/Utility/Noncopyable.hpp>
#include <Usagi/Math/Matrix.hpp>

namespace usagi
{
class Window;

class WindowManager : Noncopyable
{
public:
    virtual ~WindowManager() = default;

    virtual std::shared_ptr<Window> createWindow(
        const std::string &title,
        const Vector2i &position,
        const Vector2u32 &size
    ) = 0;

    /**
     * \brief Read window events from system event queue and dispatch
     * them to corresponding window instances.
     */
    virtual void processEvents() = 0;
};
}
