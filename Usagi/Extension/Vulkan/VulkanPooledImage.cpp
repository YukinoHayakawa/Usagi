#include "VulkanPooledImage.hpp"

#include "VulkanGpuDevice.hpp"
#include "VulkanMemoryPool.hpp"

usagi::VulkanPooledImage::VulkanPooledImage(
    vk::UniqueImage vk_image,
    const vk::Format format,
    const Vector2u32 &size,
    VulkanMemoryPool *pool,
    const std::size_t buffer_offset,
    const std::size_t buffer_size)
    : VulkanGpuImage(vk_image.getOwner(), format, size)
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

void usagi::VulkanPooledImage::upload(const void *data, const std::size_t size)
{
    // todo check size
    // assert(size <= mBufferSize);

    auto device = mPool->device();
    const auto buffer = device->allocateStageBuffer(size);
    memcpy(buffer->mappedAddress(), data, size);
    device->copyBufferToImage(buffer, this);
}
