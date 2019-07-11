#pragma once

#include <functional>

#include <Usagi/Core/Math.hpp>

#include "RenderableSystem.hpp"

namespace usagi
{
class RenderWindow;

/**
 * \brief Draw in 2D screen coordinates.
 */
class OverlayRenderingSystem : virtual public RenderableSystem
{
protected:
    using SizeFunc = std::function<Vector2f()>;
    SizeFunc mWindowSizeFunc;
    SizeFunc mFrameBufferSizeFunc;

public:
    void setWindowSizeFunc(SizeFunc func)
    {
        mWindowSizeFunc = std::move(func);
    }

    void setFrameBufferSizeFunc(SizeFunc func)
    {
        mFrameBufferSizeFunc = std::move(func);
    }

    void setSizeFunctionsFromRenderWindow(RenderWindow *render_window);
};
}
