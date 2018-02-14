#pragma once

#include <vulkan/vulkan.hpp>

#include <Usagi/Engine/Runtime/Graphics/Workload/CommandList.hpp>
#include "detail/Job.hpp"

namespace yuki::extension::vulkan
{

class CommandList : public graphics::CommandList
{
    class Device *mVulkanGD;

    detail::Job mJob;

    std::vector<vk::ImageView> mImageViews;
    std::vector<vk::ClearValue> mClearColors;
    vk::FramebufferCreateInfo mFrameBufferCreateInfo;
    vk::UniqueFramebuffer mFramebuffer;
    bool mInvalidFramebuffer = true;
    class Pipeline *mCurrentPipeline = nullptr;

    inline static vk::FenceCreateInfo FENCE_CREATE_INFO { };
    
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
    CommandList(Device *vulkan_gd, vk::UniqueCommandBuffer command_buffer);

    void begin() override;

    void bindPipeline(graphics::Pipeline *pipeline) override;

    void setViewport(float x, float y, float width, float height) override;
    void setScissor(int32_t x, int32_t y, int32_t width, int32_t height) override;

    void bindVertexBuffer(uint32_t slot, graphics::Buffer *buffer) override;

    void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) override;

    void end() override;

    vk::CommandBuffer _getCommandBuffer();
    void _setAttachments(const std::vector<class graphics::Image *> &attachments);
};

}
