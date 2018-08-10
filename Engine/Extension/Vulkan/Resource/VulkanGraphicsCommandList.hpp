#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Resource/GraphicsCommandList.hpp>

#include "VulkanBatchResource.hpp"

namespace usagi
{
class VulkanGpuCommandPool;
class VulkanGraphicsPipeline;

class VulkanGraphicsCommandList
    : public GraphicsCommandList
    , public VulkanBatchResource
{
    // this shared_ptr is used to ensure that the pool won't be freed before
    // command lists.
    std::shared_ptr<VulkanGpuCommandPool> mPool;
    vk::RenderPassBeginInfo mRenderPassBeginInfo;
    vk::UniqueCommandBuffer mCommandBuffer;
    VulkanGraphicsPipeline *mCurrentPipeline = nullptr;
    std::vector<std::shared_ptr<VulkanBatchResource>> mResources;

public:
    VulkanGraphicsCommandList(
        std::shared_ptr<VulkanGpuCommandPool> pool,
        vk::UniqueCommandBuffer vk_command_buffer);

    void beginRecording() override;
    void endRecording() override;

    void transitionImage(
        GpuImage *image,
        GpuImageLayout from,
        GpuImageLayout to,
        GraphicsPipelineStage src_stage,
        GraphicsPipelineStage dest_stage,
        GpuAccess src_access,
        GpuAccess dest_access) override;
    void clearColorImage(GpuImage *image, Color4f color) override;

    void beginRendering(
        GraphicsPipeline *pipeline,
        Framebuffer *framebuffer) override;
    void endRendering() override;

    void setViewport(
        std::uint32_t index,
        Vector2f origin,
        Vector2f size) override;

    void setScissor(
        std::uint32_t viewport_index,
        Vector2i32 origin,
        Vector2u32 size) override;

    void setConstant(
        ShaderStage stage,
        const char *name,
        const void *data,
        std::size_t size) override;

    void bindIndexBuffer(
        GpuBuffer *buffer,
        std::size_t offset,
        GraphicsIndexType type) override;

    void bindVertexBuffer(
        std::uint32_t binding_index,
        GpuBuffer *buffer,
        std::size_t offset) override;

    void drawIndexedInstanced(
        std::uint32_t index_count,
        std::uint32_t instance_count,
        std::uint32_t first_index,
        std::int32_t vertex_offset,
        std::uint32_t first_instance) override;

    vk::CommandBuffer commandBuffer() const
    {
        return mCommandBuffer.get();
    }
};
}
