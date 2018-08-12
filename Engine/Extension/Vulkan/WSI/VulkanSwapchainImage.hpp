#pragma once

#include <Usagi/Engine/Extension/Vulkan/Resource/VulkanGpuImage.hpp>
#include <Usagi/Engine/Extension/Vulkan/Resource/VulkanGpuImageView.hpp>

namespace usagi
{
class VulkanSwapchainImage : public VulkanGpuImage
{
    // obtained from the presentation engine (image)
    vk::Image mImage;
    std::shared_ptr<VulkanGpuImageView> mFullView;

public:
    VulkanSwapchainImage(
        vk::Image vk_image,
        vk::UniqueImageView full_view,
        vk::Format format);

    std::shared_ptr<GpuImageView> fullView() override;
    vk::Image image() const override { return mImage; }
};
}
