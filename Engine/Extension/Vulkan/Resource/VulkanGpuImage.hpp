#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Resource/GpuImage.hpp>

#include "VulkanGpuImageView.hpp"

namespace usagi
{
class VulkanGpuImage
    : public GpuImage
    , std::enable_shared_from_this<VulkanGpuImage>
{
    std::shared_ptr<VulkanGpuImageView> mFullView;
    vk::ImageLayout mLayout;

public:
    VulkanGpuImage(
        vk::UniqueImageView full_view,
        vk::Format format,
        vk::ImageLayout layout);

    virtual vk::Image image() const = 0;

    std::shared_ptr<GpuImageView> fullView() override;

    vk::ImageLayout layout() const
    {
        return mLayout;
    }

    void setLayout(const vk::ImageLayout layout)
    {
        mLayout = layout;
    }
};
}
