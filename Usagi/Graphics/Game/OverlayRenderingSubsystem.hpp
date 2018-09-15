#pragma once

#include <Usagi/Core/Math.hpp>

#include "RenderableSubsystem.hpp"

namespace usagi
{
/**
 * \brief Draw in 2D screen coordinates.
 */
class OverlayRenderingSubsystem : virtual public RenderableSubsystem
{
protected:
    Vector2f mWindowSize = Vector2f::Zero();
    Vector2f mFrameBufferSize = Vector2f::Zero();

public:
    /**
     * \brief Inject render size information.
     * Must be called before calling update() and render().
     * \param window_size
     * \param framebuffer_size
     */
    void setRenderSizes(
        const Vector2u32 &window_size,
        const Vector2u32 &framebuffer_size)
    {
        mWindowSize = window_size.cast<float>();
        mFrameBufferSize = framebuffer_size.cast<float>();
    }
};
}
