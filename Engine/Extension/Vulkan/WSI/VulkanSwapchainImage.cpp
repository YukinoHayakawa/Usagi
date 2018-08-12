#include "VulkanSwapchainImage.hpp"

usagi::VulkanSwapchainImage::VulkanSwapchainImage(
    std::shared_ptr<VulkanSwapchain> swapchain,
    vk::Image vk_image,
    vk::UniqueImageView vk_image_view,
    vk::Format format)
    : VulkanGpuImage(std::move(vk_image_view), format,
        vk::ImageLayout::eUndefined)
    , mSwapchain(std::move(swapchain))
    , mImage(vk_image)
{
}
