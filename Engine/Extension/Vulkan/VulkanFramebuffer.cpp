#include "VulkanFramebuffer.hpp"

#include "VulkanHelper.hpp"
#include "VulkanGpuDevice.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanGpuImageView.hpp"

usagi::VulkanFramebuffer::VulkanFramebuffer(
    VulkanGpuDevice *device,
    Vector2u32 size,
    std::vector<std::shared_ptr<VulkanGpuImageView>> views)
    : mDevice(device)
    , mSize(std::move(size))
    , mViews(std::move(views))
{
}

void usagi::VulkanFramebuffer::create(
    std::shared_ptr<VulkanRenderPass> render_pass)
{
    assert(!mFramebuffer);

    auto vk_views = vulkan::transformObjects(mViews,
        [&](auto &&v) {
            return v->view();
        }
    );

    vk::FramebufferCreateInfo fb_info;
    fb_info.setRenderPass(render_pass->renderPass());
    fb_info.setAttachmentCount(static_cast<uint32_t>(vk_views.size()));
    fb_info.setPAttachments(vk_views.data());
    fb_info.setWidth(mSize.x());
    fb_info.setHeight(mSize.y());
    fb_info.setLayers(1);

    mFramebuffer = mDevice->device().createFramebufferUnique(fb_info);
}
