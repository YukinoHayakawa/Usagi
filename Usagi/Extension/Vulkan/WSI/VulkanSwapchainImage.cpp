#include "VulkanSwapchainImage.hpp"

usagi::VulkanSwapchainImage::VulkanSwapchainImage(
    vk::Device device,
    vk::Image image,
    vk::Format format)
    : VulkanGpuImage(device, format)
    , mImage(image)
{
    createBaseView();
}
