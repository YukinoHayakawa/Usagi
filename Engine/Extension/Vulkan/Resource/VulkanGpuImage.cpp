#include "VulkanGpuImage.hpp"

usagi::VulkanGpuImage::VulkanGpuImage(
    vk::UniqueImageView full_view,
    const vk::Format format,
    vk::ImageLayout layout)
    : mFullView(std::move(full_view), format)
    , mLayout(layout)
{
}

usagi::VulkanGpuImageView * usagi::VulkanGpuImage::fullView()
{
    return &mFullView;
}
