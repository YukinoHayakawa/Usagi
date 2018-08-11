#include "VulkanGraphicsCommandList.hpp"

#include <Usagi/Engine/Utility/TypeCast.hpp>
#include <Usagi/Engine/Extension/Vulkan/VulkanGpuDevice.hpp>
#include <Usagi/Engine/Extension/Vulkan/VulkanEnumTranslation.hpp>

#include "VulkanGpuBuffer.hpp"
#include "VulkanGpuImage.hpp"
#include "VulkanFramebuffer.hpp"
#include "VulkanGpuCommandPool.hpp"
#include "VulkanBufferAllocation.hpp"
#include "VulkanMemoryPool.hpp"
#include "VulkanGraphicsPipeline.hpp"

usagi::VulkanGraphicsCommandList::VulkanGraphicsCommandList(
    std::shared_ptr<VulkanGpuCommandPool> pool,
    vk::UniqueCommandBuffer vk_command_buffer)
    : mPool(std::move(pool))
    , mCommandBuffer(std::move(vk_command_buffer))
{
}

void usagi::VulkanGraphicsCommandList::beginRecording()
{
    vk::CommandBufferBeginInfo command_buffer_begin_info;
    command_buffer_begin_info.setFlags(
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    mCommandBuffer->begin(command_buffer_begin_info);
}

void usagi::VulkanGraphicsCommandList::endRecording()
{
    mCommandBuffer->end();
}

void usagi::VulkanGraphicsCommandList::transitionImage(
    GpuImage *image,
    GpuImageLayout from,
    GpuImageLayout to,
    GraphicsPipelineStage src_stage,
    GraphicsPipelineStage dest_stage,
    GpuAccess src_access,
    GpuAccess dest_access)
{
    auto &vk_image = dynamic_cast_ref<VulkanGpuImage>(image);

    vk::ImageMemoryBarrier barrier;
    barrier.setImage(vk_image.image());
    barrier.setOldLayout(translate(from));
    const auto new_layout = translate(to);
    barrier.setNewLayout(new_layout);
    const auto queue_family_index = mPool->device()->graphicsQueueFamily();
    barrier.setSrcQueueFamilyIndex(queue_family_index);
    barrier.setDstQueueFamilyIndex(queue_family_index);
    barrier.setSrcAccessMask(translate(src_access));
    barrier.setDstAccessMask(translate(dest_access));
    vk::ImageSubresourceRange subresource_range;
    subresource_range.setAspectMask(vk::ImageAspectFlagBits::eColor);
    subresource_range.setBaseArrayLayer(0);
    subresource_range.setLayerCount(1);
    subresource_range.setBaseMipLevel(0);
    subresource_range.setLevelCount(1);
    barrier.setSubresourceRange(subresource_range);

    mCommandBuffer->pipelineBarrier(
        translate(src_stage), translate(dest_stage),
        { }, { }, { }, { barrier }
    );
    vk_image.setLayout(new_layout);
}

void usagi::VulkanGraphicsCommandList::clearColorImage(
    GpuImage *image, Color4f color)
{
    auto &vk_image = dynamic_cast_ref<VulkanGpuImage>(image);
    // todo: depending on image format, float/uint/int should be used
    const vk::ClearColorValue color_value { std::array<float, 4> {
        color.x(), color.y(), color.z(), color.w()
    }};
    vk::ImageSubresourceRange subresource_range;
    subresource_range.setAspectMask(vk::ImageAspectFlagBits::eColor);
    subresource_range.setBaseArrayLayer(0);
    subresource_range.setLayerCount(1);
    subresource_range.setBaseMipLevel(0);
    subresource_range.setLevelCount(1);
    mCommandBuffer->clearColorImage(
        vk_image.image(), vk_image.layout(),
        color_value, { subresource_range }
    );
}

void usagi::VulkanGraphicsCommandList::beginRendering(
    std::shared_ptr<GraphicsPipeline> pipeline,
    std::shared_ptr<Framebuffer> framebuffer)
{
    auto vk_framebuffer =
        dynamic_pointer_cast_throw<VulkanFramebuffer>(framebuffer);
    auto vk_pipeline =
        dynamic_pointer_cast_throw<VulkanGraphicsPipeline>(pipeline);

    vk::RenderPassBeginInfo begin_info;
    // todo support clear values
    const auto s = vk_framebuffer->size();
    begin_info.renderArea.extent.width = s.x();
    begin_info.renderArea.extent.height = s.y();
    begin_info.setFramebuffer(vk_framebuffer->framebuffer());
    begin_info.setRenderPass(vk_pipeline->renderPass());

    // assuming that only one render pass is used
    mCommandBuffer->beginRenderPass(begin_info, vk::SubpassContents::eInline);

    mCommandBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics,
        vk_pipeline->pipeline());

    mCurrentPipeline = vk_pipeline;

    mResources.push_back(std::move(vk_framebuffer));
    mResources.push_back(std::move(vk_pipeline));
}

void usagi::VulkanGraphicsCommandList::endRendering()
{
    mCommandBuffer->endRenderPass();
}

void usagi::VulkanGraphicsCommandList::setViewport(
    const std::uint32_t index,
    Vector2f origin, Vector2f size)
{
    vk::Viewport viewports[] = {
        { origin.x(), origin.y(), size.x(), size.y(), 0.f, 1.f }
    };
    mCommandBuffer->setViewport(index, 1, viewports);
}

void usagi::VulkanGraphicsCommandList::setScissor(
    const std::uint32_t viewport_index,
    Vector2i32 origin,
    Vector2u32 size)
{
    vk::Rect2D scissor {
        { origin.x(), origin.y() },
        { size.x(), size.y() }
    };
    mCommandBuffer->setScissor(viewport_index, 1, &scissor);
}

void usagi::VulkanGraphicsCommandList::setConstant(
    ShaderStage stage,
    const char *name,
    const void *data,
    const std::size_t size)
{
    if(mCurrentPipeline == nullptr)
        throw std::runtime_error(
            "No active pipeline is bound, unable to retrieve pipeline layout.");

    const auto constant_info = mCurrentPipeline->queryConstantInfo(stage, name);
    if(size != constant_info.size)
	    throw std::runtime_error("Unmatched constant size.");

    mCommandBuffer->pushConstants(
        mCurrentPipeline->layout(),
        vk::ShaderStageFlagBits::eAll,
        constant_info.offset, constant_info.size,
        data
    );
}

void usagi::VulkanGraphicsCommandList::bindIndexBuffer(
	GpuBuffer *buffer,
	const std::size_t offset,
	const GraphicsIndexType type)
{
	auto &vk_buffer = dynamic_cast_ref<VulkanGpuBuffer>(buffer);
    auto allocation = vk_buffer.allocation();

    mCommandBuffer->bindIndexBuffer(
        allocation->pool()->buffer(), allocation->offset() + offset,
        translate(type)
	);

    mResources.push_back(std::move(allocation));
}

void usagi::VulkanGraphicsCommandList::bindVertexBuffer(
	const std::uint32_t binding_index,
	GpuBuffer *buffer,
	const std::size_t offset)
{
	auto &vk_buffer = dynamic_cast_ref<VulkanGpuBuffer>(buffer);
    auto allocation = vk_buffer.allocation();

	vk::Buffer buffers[] = { allocation->pool()->buffer() };
    vk::DeviceSize sizes[] = { allocation->offset() + offset };

    mCommandBuffer->bindVertexBuffers(binding_index, 1, buffers, sizes);

    mResources.push_back(std::move(allocation));
}

void usagi::VulkanGraphicsCommandList::drawIndexedInstanced(
    const std::uint32_t index_count,
    const std::uint32_t instance_count,
    const std::uint32_t first_index,
    const std::int32_t vertex_offset,
    const std::uint32_t first_instance)
{
    mCommandBuffer->drawIndexed(index_count, instance_count, first_index,
        vertex_offset, first_instance);
}
