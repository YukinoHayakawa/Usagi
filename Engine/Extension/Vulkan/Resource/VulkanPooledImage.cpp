#include "VulkanPooledImage.hpp"

#include "VulkanMemoryPool.hpp"

usagi::VulkanPooledImage::VulkanPooledImage(
    VulkanMemoryPool *pool,
    vk::UniqueImage vk_image,
    std::size_t offset,
    std::size_t size,
    void *mapped_address)
    : mPool(pool)
    , mImage(std::move(vk_image))
    , mOffset(offset)
    , mSize(size)
    , mMappedAddress(mapped_address)
{
}

usagi::VulkanPooledImage::~VulkanPooledImage()
{
    mPool->deallocate(mOffset);
}

void usagi::VulkanPooledImage::upload(void *data, const std::size_t size)
{
    assert(size <= mSize);

    memcpy(mMappedAddress, data, size);
}

void usagi::VulkanPooledImage::createBaseView(vk::Format format)
{
    VulkanGpuImage::createBaseView(mImage.getOwner(), mImage.get(), format);
}
