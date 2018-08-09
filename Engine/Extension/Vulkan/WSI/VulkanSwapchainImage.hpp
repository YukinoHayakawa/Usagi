#pragma once

#include <Usagi/Engine/Extension/Vulkan/Resource/VulkanGpuImage.hpp>

namespace usagi
{
class VulkanGpuDevice;

class VulkanSwapchainImage : public VulkanGpuImage
{
    // obtained from the presentation engine (image)
    vk::Image mImage;
    vk::ImageLayout mLayout = vk::ImageLayout::eUndefined;

public:
    VulkanSwapchainImage(vk::Image vk_image, vk::UniqueImageView vk_image_view);

    vk::Image image() const override
    {
        return mImage;
    }

    vk::ImageLayout layout() const override
    {
        return mLayout;
    }
};
}
