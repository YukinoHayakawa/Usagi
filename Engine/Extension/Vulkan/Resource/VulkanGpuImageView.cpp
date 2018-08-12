#include "VulkanGpuImageView.hpp"

usagi::VulkanGpuImageView::VulkanGpuImageView(
    std::shared_ptr<VulkanGpuImage> image,
    vk::UniqueImageView vk_image_view,
    const vk::Format format)
    : mImage(std::move(image))
    , mImageView(std::move(vk_image_view))
    , mFormat(format)
{
}
