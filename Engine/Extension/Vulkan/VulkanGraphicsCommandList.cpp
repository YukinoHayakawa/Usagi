#include <Usagi/Engine/Runtime/Exception.hpp>

#include "VulkanGraphicsCommandList.hpp"
#include "VulkanImage.hpp"
#include "VulkanGraphicsDevice.hpp"
#include "VulkanGraphicsPipeline.hpp"

//void yuki::VulkanGraphicsCommandList::imageMemoryBarrier(GraphicsImage *image, vk::AccessFlags src_access_mask,
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

yuki::VulkanGraphicsCommandList::VulkanGraphicsCommandList(VulkanGraphicsDevice *vulkan_gd,
    vk::UniqueCommandBuffer command_buffer)
    : mVulkanGD { vulkan_gd }
    , mCommandBuffer { std::move(command_buffer) }
{
}

void yuki::VulkanGraphicsCommandList::begin(const GraphicsPipelineAssembly &assembly)
{
    VulkanGraphicsPipeline *vulkan_pipeline = dynamic_cast<VulkanGraphicsPipeline*>(assembly.pipeline);
    if(!vulkan_pipeline) throw MismatchedSubsystemComponentException() << SubsystemInfo("Rendering") << ComponentInfo("VulkanGraphicsPipeline");

    std::vector<vk::ClearValue> clear_colors;
    vk::ClearColorValue clear_color;
    clear_color.setFloat32({ 1.0f, 0.8f, 0.4f, 0.0f });

    vk::FramebufferCreateInfo framebuffer_create_info;
    framebuffer_create_info.setRenderPass(vulkan_pipeline->_getRenderPass());
    std::vector<vk::ImageView> image_views;
    for(auto &&attachment : assembly.attachments)
    {
        VulkanImage *vulkan_image = dynamic_cast<VulkanImage*>(attachment);
        if(!vulkan_image) throw MismatchedSubsystemComponentException() << SubsystemInfo("Rendering") << ComponentInfo("VulkanImage");
        image_views.push_back(vulkan_image->_getFullView());
        clear_colors.emplace_back(clear_color);
    }
    framebuffer_create_info.setAttachmentCount(image_views.size());
    framebuffer_create_info.setPAttachments(image_views.data());
    // todo real size
    framebuffer_create_info.setWidth(1280);
    framebuffer_create_info.setHeight(720);
    framebuffer_create_info.setLayers(1);
    mFramebuffer = mVulkanGD->_getDevice().createFramebufferUnique(framebuffer_create_info);

    vk::CommandBufferBeginInfo command_buffer_begin_info;
    command_buffer_begin_info.setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);

    mCommandBuffer->begin(command_buffer_begin_info);

    vk::RenderPassBeginInfo render_pass_begin_info;
    render_pass_begin_info.setRenderPass(vulkan_pipeline->_getRenderPass());
    render_pass_begin_info.setFramebuffer(mFramebuffer.get());
    render_pass_begin_info.setRenderArea({ { 0, 0 }, { 1280, 720 } }); // todo real size
    render_pass_begin_info.setClearValueCount(clear_colors.size());
    render_pass_begin_info.setPClearValues(clear_colors.data());

    mCommandBuffer->beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);
    mCommandBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics, vulkan_pipeline->_getPipeline());

    // todo transition attachment ownership
}

void yuki::VulkanGraphicsCommandList::setViewport(float x, float y, float width, float height)
{
    mCommandBuffer->setViewport(0, { { x, y, width, height, 0.f, 1.f } });
}

void yuki::VulkanGraphicsCommandList::setScissor(int32_t x, int32_t y, int32_t width, int32_t height)
{
    mCommandBuffer->setScissor(0, { { { x, y }, { static_cast<uint32_t>(width), static_cast<uint32_t>(height) } } });
}

void yuki::VulkanGraphicsCommandList::draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex,
    uint32_t first_instance)
{
    mCommandBuffer->draw(vertex_count, instance_count, first_vertex, first_instance);
}

void yuki::VulkanGraphicsCommandList::end()
{
    mCommandBuffer->endRenderPass();
    mCommandBuffer->end();
}

vk::CommandBuffer yuki::VulkanGraphicsCommandList::_getCommandBuffer() const
{
    return mCommandBuffer.get();
}
