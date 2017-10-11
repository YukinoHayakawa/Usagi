#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/GraphicsCommandList.hpp>

namespace yuki
{

class VulkanGraphicsCommandList : public GraphicsCommandList
{
    class VulkanGraphicsDevice *mVulkanGD;
    vk::UniqueCommandBuffer mCommandBuffer;
    vk::UniqueFramebuffer mFramebuffer;

    //void imageMemoryBarrier(
    //    GraphicsImage *image,
    //    vk::AccessFlags src_access_mask,
    //    vk::AccessFlags dst_access_mask,
    //    GraphicsImageLayout old_layout,
    //    GraphicsImageLayout new_layout,
    //    uint32_t src_queue_family_index,
    //    uint32_t dst_queue_family_index
    //);

public:
    VulkanGraphicsCommandList(VulkanGraphicsDevice *vulkan_gd, vk::UniqueCommandBuffer command_buffer);

    void begin(const GraphicsPipelineAssembly &assembly) override;

    void setViewport(float x, float y, float width, float height) override;
    void setScissor(int32_t x, int32_t y, int32_t width, int32_t height) override;

    void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) override;

    void end() override;

    vk::CommandBuffer _getCommandBuffer() const;
};

}
