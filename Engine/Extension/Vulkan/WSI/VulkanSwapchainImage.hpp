#pragma once

#include <Usagi/Engine/Extension/Vulkan/Resource/VulkanGpuImage.hpp>

namespace usagi
{
class VulkanSwapchain;
class VulkanGpuDevice;

class VulkanSwapchainImage : public VulkanGpuImage
{
    std::shared_ptr<VulkanSwapchain> mSwapchain;
    // obtained from the presentation engine (image)
    vk::Image mImage;

public:
    VulkanSwapchainImage(
        std::shared_ptr<VulkanSwapchain> swapchain,
        vk::Image vk_image,
        vk::UniqueImageView vk_image_view,
        vk::Format format);

    vk::Image image() const override
    {
        return mImage;
    }
};
}
