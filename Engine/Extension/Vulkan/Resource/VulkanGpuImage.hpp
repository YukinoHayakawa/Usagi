#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Resource/GpuImage.hpp>

#include "VulkanBatchResource.hpp"

namespace usagi
{
class VulkanGpuImage
    : public GpuImage
    , public VulkanBatchResource
    , public std::enable_shared_from_this<VulkanGpuImage>
{
public:
    virtual vk::Image image() const = 0;
};
}
