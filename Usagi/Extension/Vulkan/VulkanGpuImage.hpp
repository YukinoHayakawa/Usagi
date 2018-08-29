#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Runtime/Graphics/GpuImage.hpp>

#include "VulkanBatchResource.hpp"

namespace usagi
{
class VulkanGpuImageView;

class VulkanGpuImage
    : public GpuImage
    , public VulkanBatchResource
    , public std::enable_shared_from_this<VulkanGpuImage>
{
protected:
    vk::Device mDevice;
    vk::Format mFormat;
    std::shared_ptr<VulkanGpuImageView> mBaseView;
    Vector2u32 mSize;

    vk::ImageAspectFlags getAspectsFromFormat() const;
    virtual void createBaseView();

public:
    VulkanGpuImage(
        vk::Device vk_device,
        vk::Format format,
        const Vector2u32 &size);

    Vector2u32 size() const override { return mSize; }
    std::shared_ptr<GpuImageView> baseView() override;
    std::shared_ptr<GpuImageView> createView(
        const GpuImageViewCreateInfo &info) override;

    virtual vk::Image image() const = 0;
};
}
