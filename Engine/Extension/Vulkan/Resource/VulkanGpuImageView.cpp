#include "VulkanGpuImageView.hpp"

usagi::VulkanGpuImageView::VulkanGpuImageView(
    vk::UniqueImageView vk_image_view)
    : mImageView(std::move(vk_image_view))
{
}
