﻿#include "VulkanGraphicsCommandList.hpp"

#include <Usagi/Engine/Utility/TypeCast.hpp>
#include <Usagi/Engine/Extension/Vulkan/VulkanGpuDevice.hpp>
#include <Usagi/Engine/Extension/Vulkan/VulkanEnumTranslation.hpp>
#include <Usagi/Engine/Extension/Vulkan/VulkanRenderPass.hpp>

#include "VulkanGpuBuffer.hpp"
#include "VulkanGpuImage.hpp"
#include "VulkanFramebuffer.hpp"
#include "VulkanGpuCommandPool.hpp"
#include "VulkanBufferAllocation.hpp"
#include "VulkanMemoryPool.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanGpuImageView.hpp"

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
    GpuImageLayout old_layout,
    GpuImageLayout new_layout,
    GraphicsPipelineStage src_stage,
    GraphicsPipelineStage dest_stage)
{
    auto &vk_image = dynamic_cast_ref<VulkanGpuImage>(image);

    vk::ImageMemoryBarrier barrier;
    barrier.setImage(vk_image.image());
    barrier.setOldLayout(translate(old_layout));
    barrier.setNewLayout(translate(new_layout));
    const auto queue_family_index = mPool->device()->graphicsQueueFamily();
    barrier.setSrcQueueFamilyIndex(queue_family_index);
    barrier.setDstQueueFamilyIndex(queue_family_index);

    // modified from
    // https://harrylovescode.gitbooks.io/vulkan-api/content/chap07/chap07.html
    using vk::AccessFlagBits;
    switch(barrier.oldLayout)
    {
        case vk::ImageLayout::ePreinitialized:
            barrier.srcAccessMask =
                AccessFlagBits::eHostWrite | AccessFlagBits::eTransferWrite;
            break;
        case vk::ImageLayout::eColorAttachmentOptimal:
            barrier.srcAccessMask =
                AccessFlagBits::eColorAttachmentWrite;
            break;
        case vk::ImageLayout::eDepthStencilAttachmentOptimal:
            barrier.srcAccessMask =
                AccessFlagBits::eDepthStencilAttachmentWrite;
            break;
        case vk::ImageLayout::eTransferSrcOptimal:
            barrier.srcAccessMask = AccessFlagBits::eTransferRead;
            break;
        case vk::ImageLayout::eShaderReadOnlyOptimal:
            barrier.srcAccessMask = AccessFlagBits::eShaderRead;
            break;
        //case vk::ImageLayout::ePresentSrcKHR:
        //    barrier.srcAccessMask = AccessFlagBits::eMemoryRead;
        //    break;
        default: ;
    }
    switch(barrier.newLayout)
    {
        case vk::ImageLayout::eTransferDstOptimal:
            barrier.dstAccessMask = AccessFlagBits::eTransferWrite;
            break;
        case vk::ImageLayout::eTransferSrcOptimal:
            barrier.srcAccessMask |= AccessFlagBits::eTransferRead;
            barrier.dstAccessMask = AccessFlagBits::eTransferRead;
            break;
        case vk::ImageLayout::eColorAttachmentOptimal:
            barrier.dstAccessMask = AccessFlagBits::eColorAttachmentWrite;
            barrier.srcAccessMask = AccessFlagBits::eTransferRead;
            break;
        case vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal:
            barrier.dstAccessMask |=
                AccessFlagBits::eDepthStencilAttachmentWrite;
            break;
        case vk::ImageLayout::eShaderReadOnlyOptimal:
            barrier.srcAccessMask =
                AccessFlagBits::eHostWrite | AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = AccessFlagBits::eShaderRead;
            break;
        //case vk::ImageLayout::ePresentSrcKHR:
        //    barrier.dstAccessMask = AccessFlagBits::eMemoryRead;
        //    break;
        default:;
    }
    barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
    barrier.subresourceRange.setBaseArrayLayer(0);
    barrier.subresourceRange.setLayerCount(1);
    barrier.subresourceRange.setBaseMipLevel(0);
    barrier.subresourceRange.setLevelCount(1);

    mCommandBuffer->pipelineBarrier(
        translate(src_stage), translate(dest_stage),
        { }, { }, { }, { barrier }
    );
}

void usagi::VulkanGraphicsCommandList::clearColorImage(
    GpuImage *image,
    GpuImageLayout layout,
    Color4f color)
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
        vk_image.image(), translate(layout),
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
    const auto vk_renderpass = vk_pipeline->renderPass();

    // todo reuse framebuffers?
    // todo unmatched view amount?
    vk_framebuffer->create(vk_renderpass->renderPass());

    vk::RenderPassBeginInfo begin_info;
    // todo support clear values
    const auto s = vk_framebuffer->size();
    begin_info.renderArea.extent.width = s.x();
    begin_info.renderArea.extent.height = s.y();
    begin_info.setFramebuffer(vk_framebuffer->framebuffer());
    begin_info.setRenderPass(vk_renderpass->renderPass());
    const auto &clear_values = vk_renderpass->clearValues();
    begin_info.setClearValueCount(static_cast<uint32_t>(clear_values.size()));
    begin_info.setPClearValues(clear_values.data());
    // assuming that only one render pass is used
    mCommandBuffer->beginRenderPass(begin_info, vk::SubpassContents::eInline);

    mCommandBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics,
        vk_pipeline->pipeline());

    mCurrentPipeline = vk_pipeline;

    for(auto &&view : vk_framebuffer->views())
    {
        mResources.push_back(view);
        mResources.push_back(view->image()->shared_from_this());
    }
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