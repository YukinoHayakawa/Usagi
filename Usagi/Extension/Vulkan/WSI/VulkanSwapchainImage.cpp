#include "VulkanSwapchainImage.hpp"

usagi::VulkanSwapchainImage::VulkanSwapchainImage(
    const vk::Device device,
    const vk::Image image,
    const vk::Format format,
    const Vector2u32 &size)
    : VulkanGpuImage(device, format, size)
    , mImage(image)
{
    VulkanGpuImage::createBaseView();
}
