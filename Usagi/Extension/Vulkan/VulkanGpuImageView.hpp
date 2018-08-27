#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Runtime/Graphics/GpuImageView.hpp>

#include "VulkanBatchResource.hpp"
#include "VulkanShaderResource.hpp"

namespace usagi
{
class VulkanGpuImage;

class VulkanGpuImageView
    : public GpuImageView
    , public VulkanBatchResource
    , public VulkanShaderResource
{
    VulkanGpuImage *mImage;
    vk::UniqueImageView mImageView;

public:
    VulkanGpuImageView(
        VulkanGpuImage *image,
        vk::UniqueImageView vk_image_view);

    void fillShaderResourceInfo(
        vk::WriteDescriptorSet &write,
        VulkanResourceInfo &info) override;
    void appendAdditionalResources(
        std::vector<std::shared_ptr<VulkanBatchResource>> &resources) override;

    vk::ImageView view() const { return mImageView.get(); }
    VulkanGpuImage * image() const { return mImage; }
};
}
