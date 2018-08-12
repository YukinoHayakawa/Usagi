#include "VulkanSwapchainImage.hpp"

#include <Usagi/Engine/Extension/Vulkan/Resource/VulkanGpuImageView.hpp>

usagi::VulkanSwapchainImage::VulkanSwapchainImage(
    vk::Image vk_image,
    vk::UniqueImageView full_view,
    vk::Format format)
    : mImage(vk_image)
    , mFullView(std::make_shared<VulkanGpuImageView>(
        this, std::move(full_view), format))
{
}

std::shared_ptr<usagi::GpuImageView> usagi::VulkanSwapchainImage::fullView()
{
    return mFullView;
}
