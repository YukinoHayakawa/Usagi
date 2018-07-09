#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Graphics/GraphicsCommandList.hpp>

namespace usagi
{
class VulkanGraphicsPipeline;

class VulkanGraphicsCommandList : public GraphicsCommandList
{
    // Provided by VulkanGraphicsSubsystem
    vk::RenderPassBeginInfo mRenderPassBeginInfo;
    vk::CommandBuffer mCommandBuffer;
    VulkanGraphicsPipeline *mCurrentPipeline = nullptr;

public:
    explicit VulkanGraphicsCommandList(vk::CommandBuffer vk_command_buffer);

    void startRecording() override;
    void finishRecording() override;

    void bindPipeline(GraphicsPipeline *pipeline) override;

    void setViewport(
        std::uint32_t index,
        Vector2f origin,
        Vector2f size) override;

    void setScissor(
        std::uint32_t viewport_index,
        Vector2i32 origin,
        Vector2u32 size) override;

    void setConstant(
        const char *name,
        const void *data,
        std::size_t size) override;

    void drawIndexedInstanced(
        std::uint32_t index_count,
        std::uint32_t instance_count,
        std::uint32_t first_index,
        std::int32_t vertex_offset,
        std::uint32_t first_instance) override;
};
}
