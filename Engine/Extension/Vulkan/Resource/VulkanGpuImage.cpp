#include "VulkanGpuImage.hpp"

usagi::VulkanGpuImage::VulkanGpuImage(vk::UniqueImageView full_view)
    : mFullView(std::move(full_view))
{
}

usagi::VulkanGpuImageView * usagi::VulkanGpuImage::fullView()
{
    return &mFullView;
}
