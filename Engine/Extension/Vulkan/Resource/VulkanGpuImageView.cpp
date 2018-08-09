#include "VulkanGpuImageView.hpp"

usagi::VulkanGpuImageView::VulkanGpuImageView(
    vk::UniqueImageView vk_image_view,
    const vk::Format format)
    : mImageView(std::move(vk_image_view))
    , mFormat(format)
{
}
