#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Resource/GpuImage.hpp>

#include "VulkanGpuImageView.hpp"

namespace usagi
{
class VulkanGpuImage : public GpuImage
{
    VulkanGpuImageView mFullView;

public:
    explicit VulkanGpuImage(vk::UniqueImageView full_view);

    virtual vk::Image image() const = 0;
    virtual vk::ImageLayout layout() const = 0;

    VulkanGpuImageView * fullView() override;
};
}
