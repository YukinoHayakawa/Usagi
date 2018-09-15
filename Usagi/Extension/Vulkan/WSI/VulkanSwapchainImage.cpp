#include "VulkanSwapchainImage.hpp"

usagi::VulkanSwapchainImage::VulkanSwapchainImage(
    GpuImageFormat format,
    const Vector2u32 &size,
    vk::Device vk_device,
    vk::Image vk_image)
    : VulkanGpuImage(std::move(format), size, std::move(vk_device))
    , mImage(std::move(vk_image))
{
    VulkanGpuImage::createBaseView();
}
