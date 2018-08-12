#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Resource/Framebuffer.hpp>

#include "VulkanBatchResource.hpp"

namespace usagi
{
class VulkanGpuImageView;
class VulkanGpuDevice;
class VulkanRenderPass;

class VulkanFramebuffer
    : public Framebuffer
    , public VulkanBatchResource
{
    VulkanGpuDevice *mDevice = nullptr;
    Vector2u32 mSize;
    std::vector<std::shared_ptr<VulkanGpuImageView>> mViews;
    vk::UniqueFramebuffer mFramebuffer;

public:
    VulkanFramebuffer(
        VulkanGpuDevice *device,
        Vector2u32 size,
        std::vector<std::shared_ptr<VulkanGpuImageView>> views);

    Vector2u32 size() const override { return mSize; }

    void create(vk::RenderPass render_pass);

    vk::Framebuffer framebuffer() const { return mFramebuffer.get(); }

    const std::vector<std::shared_ptr<VulkanGpuImageView>> & views() const
    {
        return mViews;
    }
};
}
