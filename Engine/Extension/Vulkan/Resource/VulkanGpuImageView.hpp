#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Resource/GpuImageView.hpp>

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
    vk::Format mFormat;

public:
    VulkanGpuImageView(
        VulkanGpuImage *image,
        vk::UniqueImageView vk_image_view,
        vk::Format format);

    void fillShaderResourceInfo(
        vk::WriteDescriptorSet &write,
        VulkanResourceInfo &info) override;
    void appendAdditionalResources(
        std::vector<std::shared_ptr<VulkanBatchResource>> &resources) override;

    vk::ImageView view() const { return mImageView.get(); }
    vk::Format format() const { return mFormat; }
    VulkanGpuImage * image() const { return mImage; }
};
}
