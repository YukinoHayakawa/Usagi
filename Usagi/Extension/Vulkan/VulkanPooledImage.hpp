#pragma once

#include "VulkanGpuImage.hpp"

namespace usagi
{
class VulkanMemoryPool;

class VulkanPooledImage : public VulkanGpuImage
{
    VulkanMemoryPool *mPool = nullptr;
    vk::UniqueImage mImage;
    std::size_t mOffset;
    std::size_t mSize;
    void *mMappedAddress = nullptr;

    friend class VulkanMemoryPool;
    void createBaseView();

public:
    VulkanPooledImage(
        VulkanMemoryPool *pool,
        vk::UniqueImage vk_image,
        vk::Format format,
        std::size_t offset,
        std::size_t size,
        void *mapped_address);
    ~VulkanPooledImage();

    void upload(const void *data, std::size_t size) override;

    vk::Image image() const override { return mImage.get(); }
    std::size_t offset() const { return mOffset; }
};
}
