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
     * \param buf_data
     * \param buf_size
     */
    virtual void upload(const void *buf_data, std::size_t buf_size) = 0;
    // todo uploading from subregion of a complete buffer to subregion of an image
    virtual void uploadRegion(
        const void *buf_data,
        std::size_t buf_size,
        const Vector2i &tex_offset,
        const Vector2u32 &tex_size) = 0;
};
}
