#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Runtime/Graphics/Resource/GraphicsCommandList.hpp>

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
    std::shared_ptr<VulkanGpuCommandPool> mCommandPool;
    vk::UniqueCommandBuffer mCommandBuffer;
    std::shared_ptr<VulkanGraphicsPipeline> mCurrentPipeline;
    std::vector<vk::UniqueDescriptorPool> mDescriptorPools;
    // whole pool is discarded after use, so unique handles are not used.
    std::vector<vk::DescriptorSet> mDescriptorSets;
    std::vector<std::shared_ptr<VulkanBatchResource>> mResources;

    void createDescriptorPool();
    vk::DescriptorSet allocateDescriptorSet(std::uint32_t set_id);

public:
    VulkanGraphicsCommandList(
        std::shared_ptr<VulkanGpuCommandPool> pool,
        vk::UniqueCommandBuffer vk_command_buffer);

    void beginRecording() override;
    void endRecording() override;

    void imageTransition(
        GpuImage *image,
        GpuImageLayout old_layout,
        GpuImageLayout new_layout,
        GraphicsPipelineStage src_stage,
        GraphicsPipelineStage dest_stage) override;
    void clearColorImage(
        GpuImage *image,
        GpuImageLayout layout,
        Color4f color) override;

    void beginRendering(
        std::shared_ptr<RenderPass> render_pass,
        std::shared_ptr<Framebuffer> framebuffer) override;
    void endRendering() override;

    void bindPipeline(std::shared_ptr<GraphicsPipeline> pipeline) override;

    void bindResourceSet(
        std::uint32_t set_id,
        std::initializer_list<std::shared_ptr<ShaderResource>> resources
    ) override;

    void setViewport(
        std::uint32_t index,
        Vector2f origin,
        Vector2f size) override;
    void setScissor(
        std::uint32_t viewport_index,
        Vector2i32 origin,
        Vector2u32 size) override;
    void setLineWidth(float width) override;

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

    void drawInstanced(
        std::uint32_t vertex_count,
        std::uint32_t instance_count,
        std::uint32_t first_vertex,
        std::uint32_t first_instance) override;
    void drawIndexedInstanced(
        std::uint32_t index_count,
        std::uint32_t instance_count,
        std::uint32_t first_index,
        std::int32_t vertex_offset,
        std::uint32_t first_instance) override;

    vk::CommandBuffer commandBuffer() const { return mCommandBuffer.get(); }
};
}
