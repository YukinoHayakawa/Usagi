#pragma once

#include "VulkanGpuImage.hpp"

namespace usagi
{
class VulkanMemoryPool;

class VulkanPooledImage : public VulkanGpuImage
{
    vk::UniqueImage mImage;
    VulkanMemoryPool *mPool = nullptr;
    std::size_t mBufferOffset;
    std::size_t mBufferSize;

    friend class VulkanMemoryPool;

public:
    VulkanPooledImage(
        vk::UniqueImage vk_image,
        GpuImageFormat format,
        const Vector2u32 &size,
        VulkanMemoryPool *pool,
        std::size_t buffer_offset,
        std::size_t buffer_size);
    ~VulkanPooledImage();

    void upload(const void *buf_data, std::size_t buf_size) override;

    void uploadRegion(
        const void *buf_data,
        std::size_t buf_size,
        const Vector2i &tex_offset,
        const Vector2u32 &tex_size) override;

    vk::Image image() const override { return mImage.get(); }
    std::size_t offset() const { return mBufferOffset; }
};
}
