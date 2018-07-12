#include "VulkanGraphicsCommandList.hpp"

#include <loguru.hpp>

#include "VulkanGraphicsPipeline.hpp"
#include "VulkanGpuBuffer.hpp"

vk::IndexType usagi::VulkanGraphicsCommandList::translateIndexType(
    const GraphicsIndexType type)
{
    switch(type)
    {
        case GraphicsIndexType::UINT16:
            return vk::IndexType::eUint16;
        case GraphicsIndexType::UINT32:
            return vk::IndexType::eUint32;
        default:
            throw std::runtime_error("Invalid index type.");
    }
}

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
    auto &vk_pipeline = dynamic_cast<VulkanGraphicsPipeline&>(*pipeline);

    mCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
        vk_pipeline.pipeline());

    mCurrentPipeline = &vk_pipeline;
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
    ShaderStage stage,
    const char *name,
    const void *data,
    const std::size_t size)
{
    if(mCurrentPipeline == nullptr)
        throw std::runtime_error(
            "No active pipeline is bound, unable to retrieve pipeline layout.");

    const auto constant_info = mCurrentPipeline->queryConstantInfo(stage, name);
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

void usagi::VulkanGraphicsCommandList::bindIndexBuffer(
	GpuBuffer *buffer,
	std::size_t offset,
	GraphicsIndexType type)
{
	auto &vk_buffer = dynamic_cast<VulkanGpuBuffer&>(*buffer);

    mCommandBuffer.bindIndexBuffer(
		vk_buffer.buffer(), offset,
        translateIndexType(type)
	);
}

void usagi::VulkanGraphicsCommandList::bindVertexBuffer(
	std::uint32_t binding_index,
	GpuBuffer *buffer,
	std::size_t offset)
{
	auto &vk_buffer = dynamic_cast<VulkanGpuBuffer&>(*buffer);

	vk::Buffer buffers[] = { vk_buffer.buffer() };
    vk::DeviceSize sizes[] = { offset };

    mCommandBuffer.bindVertexBuffers(binding_index, 1, buffers, sizes);
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
