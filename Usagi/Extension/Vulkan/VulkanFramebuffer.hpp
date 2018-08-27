#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include <Usagi/Runtime/Graphics/Resource/Framebuffer.hpp>

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
    std::shared_ptr<VulkanRenderPass> mRenderPass;

public:
    VulkanFramebuffer(
        VulkanGpuDevice *device,
        Vector2u32 size,
        std::vector<std::shared_ptr<VulkanGpuImageView>> views);

    Vector2u32 size() const override { return mSize; }

    void create(std::shared_ptr<VulkanRenderPass> render_pass);

    vk::Framebuffer framebuffer() const { return mFramebuffer.get(); }

    const std::vector<std::shared_ptr<VulkanGpuImageView>> & views() const
    {
        return mViews;
    }
};
}
