#include "VulkanGpuImage.hpp"

usagi::VulkanGpuImage::VulkanGpuImage(
    vk::UniqueImageView full_view,
    const vk::Format format,
    vk::ImageLayout layout)
    : mFullView(std::make_shared<VulkanGpuImageView>(
        shared_from_this(), std::move(full_view), format))
    , mLayout(layout)
{
}

std::shared_ptr<usagi::GpuImageView> usagi::VulkanGpuImage::fullView()
{
    return mFullView;
}
