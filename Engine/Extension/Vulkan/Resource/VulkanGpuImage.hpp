#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Resource/GpuImage.hpp>

#include "VulkanBatchResource.hpp"

namespace usagi
{
class VulkanGpuImageView;

class VulkanGpuImage
    : public GpuImage
    , public VulkanBatchResource
    , public std::enable_shared_from_this<VulkanGpuImage>
{
protected:
    std::shared_ptr<VulkanGpuImageView> mBaseView;

    void createBaseView(vk::Device device, vk::Image image, vk::Format format);

public:
    std::shared_ptr<GpuImageView> baseView() override;
    virtual vk::Image image() const = 0;
};
}
