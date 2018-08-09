#pragma once

#include <memory>

#include <Usagi/Engine/Utility/Noncopyable.hpp>
#include <Usagi/Engine/Core/Math.hpp>

#include "Enum/GpuBufferFormat.hpp"

namespace usagi
{
class GpuImage;
class GpuSemaphore;

class Swapchain : Noncopyable
{
public:
    virtual ~Swapchain() = default;

    virtual void create(GpuBufferFormat format) = 0;

    /**
     * \brief If desired format is not available, swapchain may fallback to
     * other format than the requested.
     * \return
     */
    virtual GpuBufferFormat format() const = 0;
    virtual Vector2u32 size() const = 0;

    /**
     * \brief A semaphore signaled when the image becomes available.
     * Must not be destroyed until signaled.
     * \return
     */
    virtual std::shared_ptr<GpuSemaphore> acquireNextImage() = 0;
    virtual GpuImage * currentImage() = 0;

    virtual void present(
        std::initializer_list<std::shared_ptr<GpuSemaphore>> wait_semaphores
    ) =0;
};
}
