#include <Usagi/Engine/Runtime/Exception.hpp>

#include "CommandList.hpp"
#include "../Resource/Image.hpp"
#include "../Device/Device.hpp"
#include "../Workload/Pipeline.hpp"
#include "../Resource/Buffer.hpp"

namespace yuki::vulkan
{

//void CommandList::imageMemoryBarrier(GraphicsImage *image, vk::AccessFlags src_access_mask,
//    vk::AccessFlags dst_access_mask, GraphicsImageLayout old_layout, GraphicsImageLayout new_layout,
//    uint32_t src_queue_family_index, uint32_t dst_queue_family_index)
//{
//    VulkanImage *vulkan_image = dynamic_cast<VulkanImage*>(image);
//    if(!vulkan_image) throw MismatchedSubsystemComponentException() << SubsystemInfo("Rendering") << ComponentInfo("VulkanImage");
//
//    vk::ImageSubresourceRange image_subresource_range;
//    image_subresource_range.setAspectMask(vk::ImageAspectFlagBits::eColor);
//    image_subresource_range.setBaseMipLevel(0);
//    image_subresource_range.setLevelCount(1);
//    image_subresource_range.setBaseArrayLayer(0);
//    image_subresource_range.setLayerCount(1);
//
//    vk::ImageMemoryBarrier barrier;
//    barrier.setSrcAccessMask(src_access_mask);
//    barrier.setDstAccessMask(dst_access_mask);
//    // layout transition
//    barrier.setOldLayout(VulkanImage::translateImageLayout(old_layout));
//    barrier.setNewLayout(VulkanImage::translateImageLayout(new_layout));
//    // ownership transfer
//    barrier.setSrcQueueFamilyIndex(src_queue_family_index);
//    barrier.setDstQueueFamilyIndex(dst_queue_family_index);
//    barrier.setImage(vulkan_image->_getImageHandle());
//    barrier.setSubresourceRange(image_subresource_range);
//
//    mCommandBuffer->pipelineBarrier()
//}

CommandList::CommandList(Device *vulkan_gd,
    vk::UniqueCommandBuffer command_buffer)
    : mVulkanGD{ vulkan_gd }
    , mCommandBuffer{ std::move(command_buffer) }
{
}

void CommandList::begin()
{
    if(mInvalidFramebuffer) throw InappropriatelyConfiguredException() << ConfigureInfo("render targets are not specified");

    mCurrentPipeline = nullptr;

    vk::CommandBufferBeginInfo command_buffer_begin_info;
    command_buffer_begin_info.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    mCommandBuffer->begin(command_buffer_begin_info);

    // todo transition attachment ownership of swapchain image from present queue to render queue
}

void CommandList::bindPipeline(graphics::Pipeline *pipeline)
{
    Pipeline *vulkan_pipeline = dynamic_cast<Pipeline*>(pipeline);
    if(!vulkan_pipeline) throw MismatchedSubsystemComponentException() << SubsystemInfo("Rendering") << ComponentInfo("VulkanGraphicsPipeline");

    if(vulkan_pipeline == mCurrentPipeline) return;

    // unbind last pipeline, assume all resource descriptors are unbound from now on
    if(mCurrentPipeline != nullptr)
    {
        mCommandBuffer->endRenderPass();
    }

    mFrameBufferCreateInfo.setRenderPass(vulkan_pipeline->_getRenderPass());
    mFramebuffer = mVulkanGD->_getDevice().createFramebufferUnique(mFrameBufferCreateInfo);

    // todo: decouple render pass from pipeline and avoid changes of render passes as much as possible
    vk::RenderPassBeginInfo render_pass_begin_info;
    render_pass_begin_info.setRenderPass(vulkan_pipeline->_getRenderPass());
    render_pass_begin_info.setFramebuffer(mFramebuffer.get());
    // todo: real size
    render_pass_begin_info.setRenderArea({ { 0, 0 }, { 1280, 720 } });
    render_pass_begin_info.setClearValueCount(mClearColors.size());
    render_pass_begin_info.setPClearValues(mClearColors.data());

    mCommandBuffer->beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);
    mCommandBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics, vulkan_pipeline->_getPipeline());

    mCurrentPipeline = vulkan_pipeline;
}

void CommandList::setViewport(float x, float y, float width, float height)
{
    mCommandBuffer->setViewport(0, { { x, y, width, height, 0.f, 1.f } });
}

void CommandList::setScissor(int32_t x, int32_t y, int32_t width, int32_t height)
{
    mCommandBuffer->setScissor(0, { { { x, y }, { static_cast<uint32_t>(width), static_cast<uint32_t>(height) } } });
}

void CommandList::bindVertexBuffer(uint32_t slot, graphics::Buffer *buffer)
{
    Buffer *vulkan_buffer = dynamic_cast<Buffer*>(buffer);
    if(!vulkan_buffer) throw MismatchedSubsystemComponentException() << SubsystemInfo("Rendering") << ComponentInfo("VulkanVertexBuffer");

    mCommandBuffer->bindVertexBuffers(slot, { vulkan_buffer->_getBuffer() }, { vulkan_buffer->_getOffset() });
}

void CommandList::draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex,
    uint32_t first_instance)
{
    mCommandBuffer->draw(vertex_count, instance_count, first_vertex, first_instance);
}

void CommandList::end()
{
    if(mCurrentPipeline) mCommandBuffer->endRenderPass();
    mCurrentPipeline = nullptr;
    mCommandBuffer->end();

    mInvalidFramebuffer = true;
}

vk::CommandBuffer CommandList::_getCommandBuffer() const
{
    return mCommandBuffer.get();
}

void CommandList::_setAttachments(const std::vector<graphics::Image *> &attachments)
{
    mImageViews.clear();
    mClearColors.clear();

    vk::ClearColorValue clear_color;
    clear_color.setFloat32({ 1.0f, 0.8f, 0.4f, 0.0f });

    // mFrameBufferCreateInfo.setRenderPass();
    for(auto &&attachment : attachments)
    {
        Image *vulkan_image = dynamic_cast<Image*>(attachment);
        if(!vulkan_image) throw MismatchedSubsystemComponentException() << SubsystemInfo("Rendering") << ComponentInfo("VulkanImage");
        mImageViews.push_back(vulkan_image->_getFullView());
        mClearColors.emplace_back(clear_color);
    }
    mFrameBufferCreateInfo.setAttachmentCount(mImageViews.size());
    mFrameBufferCreateInfo.setPAttachments(mImageViews.data());
    // todo real size
    mFrameBufferCreateInfo.setWidth(1280);
    mFrameBufferCreateInfo.setHeight(720);
    mFrameBufferCreateInfo.setLayers(1);
    // mFramebuffer = mVulkanGD->_getDevice().createFramebufferUnique(framebuffer_create_info);

    mInvalidFramebuffer = false;
}

}
