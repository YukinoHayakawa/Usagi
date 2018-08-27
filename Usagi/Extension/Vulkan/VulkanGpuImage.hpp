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

    vk::ImageAspectFlags getAspectsFromFormat() const;
    void createBaseView();

public:
    VulkanGpuImage(vk::Device vk_device, vk::Format format);

    std::shared_ptr<GpuImageView> baseView() override;
    std::shared_ptr<GpuImageView> createView(
        const GpuImageViewCreateInfo &info) override;

    virtual vk::Image image() const = 0;
};
}
