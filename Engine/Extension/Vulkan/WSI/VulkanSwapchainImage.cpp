#include "VulkanSwapchainImage.hpp"

usagi::VulkanSwapchainImage::VulkanSwapchainImage(
    vk::Image vk_image,
    vk::UniqueImageView vk_image_view,
    vk::Format format)
    : VulkanGpuImage(std::move(vk_image_view), format,
        vk::ImageLayout::eUndefined)
    , mImage(vk_image)
{
}
