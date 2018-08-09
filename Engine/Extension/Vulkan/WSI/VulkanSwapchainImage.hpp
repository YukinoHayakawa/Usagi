#pragma once

#include <Usagi/Engine/Extension/Vulkan/Resource/VulkanGpuImage.hpp>

namespace usagi
{
class VulkanGpuDevice;

class VulkanSwapchainImage : public VulkanGpuImage
{
    // obtained from the presentation engine (image)
    vk::Image mImage;

public:
    VulkanSwapchainImage(
        vk::Image vk_image,
        vk::UniqueImageView vk_image_view,
        vk::Format format);

    vk::Image image() const override
    {
        return mImage;
    }
};
}
