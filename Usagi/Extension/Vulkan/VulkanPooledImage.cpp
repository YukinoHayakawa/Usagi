#include "VulkanPooledImage.hpp"

#include <Usagi/Runtime/Graphics/GraphicsCommandList.hpp>

#include "VulkanGpuDevice.hpp"
#include "VulkanMemoryPool.hpp"
#include "VulkanGpuCommandPool.hpp"

usagi::VulkanPooledImage::VulkanPooledImage(
    VulkanMemoryPool *pool,
    vk::UniqueImage vk_image,
    vk::Format format,
    std::size_t offset,
    std::size_t size,
    void *mapped_address)
    : VulkanGpuImage(vk_image.getOwner(), format)
    , mPool(pool)
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

void usagi::VulkanPooledImage::upload(const void *data, const std::size_t size)
{
    assert(size <= mSize);

    auto gpu_device = mPool->device();
    // todo streaming & staging
    auto pool = gpu_device->createCommandPool();

    memcpy(mMappedAddress, data, size);

    vk::MappedMemoryRange range;
    range.setMemory(mPool->memory());
    range.setOffset(mOffset);
    range.setSize(mSize);
    mPool->device()->device().flushMappedMemoryRanges({ range });

    auto post = pool->allocateGraphicsCommandList();
    post->beginRecording();
    post->imageTransition(
        this,
        GpuImageLayout::PREINITIALIZED,
        GpuImageLayout::SHADER_READ_ONLY,
        GraphicsPipelineStage::HOST,
        GraphicsPipelineStage::FRAGMENT_SHADER
    );
    post->endRecording();
    gpu_device->submitGraphicsJobs({ post }, { }, { }, { });
  //  gpu_device->waitIdle();
}

void usagi::VulkanPooledImage::createBaseView()
{
    VulkanGpuImage::createBaseView();
}
