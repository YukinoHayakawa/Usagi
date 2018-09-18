#pragma once

#include <memory>

#include <Usagi/Utility/Noncopyable.hpp>
#include <Usagi/Core/Math.hpp>

#include "Enum/GpuBufferFormat.hpp"

namespace usagi
{
class GpuImage;
class GpuSemaphore;

class Swapchain : Noncopyable
{
public:
    virtual ~Swapchain() = default;

    /**
     * \brief Create the swapchain with a preferret format. If it is not
     * supported by the device, a fallback format may be used, which can
     * be queried using format(). If the swapchain is already created, it
     * will be recreated.
     * \param size
     * \param format
     */
    virtual void create(const Vector2u32 &size, GpuBufferFormat format) = 0;

    /**
     * \brief Recreate the swapchain with the same format and different
     * resolution.
     * \param size
     */
    virtual void resize(const Vector2u32 &size) = 0;

    /**
     * \brief If desired format is not available, swapchain may fallback to
     * other format than the requested.
     * \return
     */
    virtual GpuBufferFormat format() const = 0;
    virtual Vector2u32 size() const = 0;

    float aspectRatio() const
    {
        const auto s = size();
        return static_cast<float>(s.x()) / s.y();
    }

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
