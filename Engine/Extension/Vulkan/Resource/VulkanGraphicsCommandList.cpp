#include "VulkanGraphicsCommandList.hpp"

#include <Usagi/Engine/Utility/TypeCast.hpp>

#include "../VulkanGraphicsPipeline.hpp"
#include "../VulkanEnumTranslation.hpp"
#include "VulkanGpuBuffer.hpp"
#include "VulkanGpuImage.hpp"
#include "VulkanFramebuffer.hpp"

usagi::VulkanGraphicsCommandList::VulkanGraphicsCommandList(
    vk::UniqueCommandBuffer vk_command_buffer)
    : mCommandBuffer { std::move(vk_command_buffer) }
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

void usagi::VulkanGraphicsCommandList::clearColorImage(
    GpuImage *image, Color4f color)
{
    auto &vk_image = dynamic_cast_ref<VulkanGpuImage>(image);
    const vk::ClearColorValue color_value { std::array<float, 4> {
        color.x(), color.y(), color.z(), color.w()
    }};
    mCommandBuffer->clearColorImage(
        vk_image.image(), vk_image.layout(),
        color_value, {}
    );
}

void usagi::VulkanGraphicsCommandList::beginRendering(
    GraphicsPipeline *pipeline,
    Framebuffer *framebuffer)
{
    auto &vk_framebuffer = dynamic_cast_ref<VulkanFramebuffer>(framebuffer);
    auto &vk_pipeline = dynamic_cast_ref<VulkanGraphicsPipeline>(pipeline);

    vk::RenderPassBeginInfo begin_info;
    // todo support clear values
    const auto s = vk_framebuffer.size();
    begin_info.renderArea.extent.width = s.x();
    begin_info.renderArea.extent.height = s.y();
    begin_info.setFramebuffer(vk_framebuffer.framebuffer());
    begin_info.setRenderPass(vk_pipeline.renderPass());

    // assuming that only one render pass is used
    mCommandBuffer->beginRenderPass(mRenderPassBeginInfo,
        vk::SubpassContents::eInline);

    mCommandBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics,
        vk_pipeline.pipeline());

    mCurrentPipeline = &vk_pipeline;
};

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

    mCommandBuffer->bindIndexBuffer(
		vk_buffer.buffer(), offset,
        translate(type)
	);
}

void usagi::VulkanGraphicsCommandList::bindVertexBuffer(
	const std::uint32_t binding_index,
	GpuBuffer *buffer,
	const std::size_t offset)
{
	auto &vk_buffer = dynamic_cast_ref<VulkanGpuBuffer>(buffer);

	vk::Buffer buffers[] = { vk_buffer.buffer() };
    vk::DeviceSize sizes[] = { offset };

    mCommandBuffer->bindVertexBuffers(binding_index, 1, buffers, sizes);
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
