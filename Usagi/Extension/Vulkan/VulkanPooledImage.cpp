#include "VulkanPooledImage.hpp"

#include "VulkanGpuDevice.hpp"
#include "VulkanMemoryPool.hpp"

usagi::VulkanPooledImage::VulkanPooledImage(
    vk::UniqueImage vk_image,
    GpuImageFormat format,
    const Vector2u32 &size,
    VulkanMemoryPool *pool,
    const std::size_t buffer_offset,
    const std::size_t buffer_size)
    : VulkanGpuImage(format, size, vk_image.getOwner())
    , mImage(std::move(vk_image))
    , mPool(pool)
    , mBufferOffset(buffer_offset)
    , mBufferSize(buffer_size)
{
}

usagi::VulkanPooledImage::~VulkanPooledImage()
{
    mPool->deallocate(mBufferOffset);
}

void usagi::VulkanPooledImage::upload(
    const void *buf_data,
    const std::size_t buf_size)
{
    uploadRegion(buf_data, buf_size, Vector2i::Zero(), mSize);
}

void usagi::VulkanPooledImage::uploadRegion(
    const void *buf_data,
    std::size_t buf_size,
    const Vector2i &tex_offset,
    const Vector2u32 &tex_size)
{
    assert(buf_size <= mBufferSize);

    auto device = mPool->device();
    const auto buffer = device->allocateStageBuffer(buf_size);
    memcpy(buffer->mappedAddress(), buf_data, buf_size);
    device->copyBufferToImage(buffer, this, tex_offset, tex_size);
}
