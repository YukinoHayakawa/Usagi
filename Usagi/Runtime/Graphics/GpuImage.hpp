#pragma once

#include <memory>

#include <Usagi/Utility/Noncopyable.hpp>
#include <Usagi/Core/Math.hpp>

#include "GpuImageFormat.hpp"

namespace usagi
{
struct GpuImageViewCreateInfo;
class GpuImageView;

class GpuImage : Noncopyable
{
protected:
    GpuImageFormat mFormat;
    Vector2u32 mSize;

public:
    GpuImage(GpuImageFormat format, const Vector2u32 &size)
        : mFormat(std::move(format))
        , mSize(std::move(size))
    {
    }

    virtual ~GpuImage() = default;

    GpuImageFormat format() const { return mFormat; }
    Vector2u32 size() const { return mSize; }
    virtual std::shared_ptr<GpuImageView> baseView() = 0;
    virtual std::shared_ptr<GpuImageView> createView(
        const GpuImageViewCreateInfo &info) = 0;

    /**
     * \brief Upload image data to GPU memory.
     * \param data
     * \param size
     */
    virtual void upload(const void *data, std::size_t size) = 0;
};
}
