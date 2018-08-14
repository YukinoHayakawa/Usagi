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

public:
    VulkanPooledImage(
        VulkanMemoryPool *pool,
        vk::UniqueImage vk_image,
        std::size_t offset,
        std::size_t size,
        void *mapped_address);
    ~VulkanPooledImage();

    void upload(void *data, std::size_t size) override;

    void createBaseView(vk::Format format);
    vk::Image image() const override { return mImage.get(); }
    std::size_t offset() const { return mOffset; }
};
}
