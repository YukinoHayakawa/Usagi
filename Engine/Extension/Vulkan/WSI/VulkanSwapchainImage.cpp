#include "VulkanSwapchainImage.hpp"

usagi::VulkanSwapchainImage::VulkanSwapchainImage(
    vk::Device device,
    vk::Image image,
    vk::Format format)
    : mImage(image)
{
    createBaseView(device, image, format);
}
