#include "VulkanGraphicsCommandList.hpp"

#include "VulkanGraphicsPipeline.hpp"

#include <loguru.hpp>

usagi::VulkanGraphicsCommandList::VulkanGraphicsCommandList(
    const vk::CommandBuffer vk_command_buffer)
    : mCommandBuffer { vk_command_buffer } {}

void usagi::VulkanGraphicsCommandList::startRecording()
{
    vk::CommandBufferBeginInfo command_buffer_begin_info;
    command_buffer_begin_info.setFlags(
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    mCommandBuffer.begin(command_buffer_begin_info);

    // assuming that only one render pass is used
    mCommandBuffer.beginRenderPass(mRenderPassBeginInfo,
        vk::SubpassContents::eInline);
}

void usagi::VulkanGraphicsCommandList::finishRecording()
{
    mCommandBuffer.endRenderPass();
    mCommandBuffer.end();
}

void usagi::VulkanGraphicsCommandList::bindPipeline(GraphicsPipeline *pipeline)
{
    const auto vk_pipeline = dynamic_cast<VulkanGraphicsPipeline*>(pipeline);

    mCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
        vk_pipeline->pipeline());

    mCurrentPipeline = vk_pipeline;
}

void usagi::VulkanGraphicsCommandList::setViewport(
    const std::uint32_t index,
    Vector2f origin,
    Vector2f size)
{
    vk::Viewport viewports[] = {
        { origin.x(), origin.y(), size.x(), size.y(), 0.f, 1.f }
    };
    mCommandBuffer.setViewport(index, 1, viewports);
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
    mCommandBuffer.setScissor(viewport_index, 1, &scissor);
}

void usagi::VulkanGraphicsCommandList::setConstant(
    const char *name,
    const void *data,
    const std::size_t size)
{
    if(mCurrentPipeline == nullptr)
        throw std::runtime_error(
            "No active pipeline is bound, unable to retrieve pipeline layout.");

    const auto constant_info = mCurrentPipeline->queryConstantInfo(name);
	CHECK_F(size == constant_info.size,
		"Unmatched constant size: input is %zubytes, "
		"but the pipeline layout says it should be %zu bytes.",
		size, constant_info.size);
    mCommandBuffer.pushConstants(
        mCurrentPipeline->layout(),
        vk::ShaderStageFlagBits::eAll,
        constant_info.offset, constant_info.size,
        data
    );
}

void usagi::VulkanGraphicsCommandList::drawIndexedInstanced(
    const std::uint32_t index_count,
    const std::uint32_t instance_count,
    const std::uint32_t first_index,
    const std::int32_t vertex_offset,
    const std::uint32_t first_instance)
{
    mCommandBuffer.drawIndexed(index_count, instance_count, first_index,
        vertex_offset, first_instance);
}
