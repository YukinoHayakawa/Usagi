#include "VulkanFramebuffer.hpp"

usagi::VulkanFramebuffer::VulkanFramebuffer(
    std::shared_ptr<VulkanRenderPass> vulkan_render_pass,
    vk::UniqueFramebuffer vk_framebuffer,
    const Vector2u32 &size)
    : mRenderPass(std::move(vulkan_render_pass))
    , mFramebuffer(std::move(vk_framebuffer))
    , mSize(size)
{
}
