#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Resource/Framebuffer.hpp>

namespace usagi
{
class VulkanRenderPass;

class VulkanFramebuffer : public Framebuffer
{
    std::shared_ptr<VulkanRenderPass> mRenderPass;
    vk::UniqueFramebuffer mFramebuffer;
    Vector2u32 mSize;

public:
    VulkanFramebuffer(
        std::shared_ptr<VulkanRenderPass> vulkan_render_pass,
        vk::UniqueFramebuffer vk_framebuffer,
        const Vector2u32 &size);

    Vector2u32 size() const override { return mSize; }

    vk::Framebuffer framebuffer() const
    {
        return mFramebuffer.get();
    }
};
}
