#include "VulkanSwapchainImage.hpp"

usagi::VulkanSwapchainImage::VulkanSwapchainImage(
    vk::Image vk_image,
    vk::UniqueImageView vk_image_view)
    : VulkanGpuImage(std::move(vk_image_view))
    , mImage(vk_image)
{
}
