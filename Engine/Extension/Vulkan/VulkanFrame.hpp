#pragma once

#include <Usagi/Engine/Runtime/Graphics/GraphicsFrame.hpp>
#include <Usagi/Engine/Runtime/Graphics/GraphicsResourceAllocator.hpp>
#include "VulkanMemoryAllocator.hpp"
#include "VulkanSemaphore.hpp"

namespace yuki
{

class VulkanFrame : public GraphicsFrame
{
    class VulkanGraphicsDevice *mDevice;

    std::unique_ptr<class VulkanGraphicsCommandPool> mCommandPool;
    std::unique_ptr<class VulkanGraphicsCommandList> mCommandList;

    vk::Framebuffer mFramebuffer;
    VulkanSemaphore mRenderingFinishedSemaphore;
    vk::UniqueFence mFrameFinishedFence;
    VulkanMemoryAllocator mFrameStackAllocator; // reset on every frame

public:
    explicit VulkanFrame(VulkanGraphicsDevice *device);

    void beginFrame(const std::vector<GraphicsImage *> &attachments) override;

    GraphicsCommandList * getCommandList() override;
    GraphicsResourceAllocator * getResourceAllocator() override;
    GraphicsSemaphore * getRenderingFinishedSemaphore() override;

    void endFrame() override;

    VulkanMemoryAllocator *_getStackMemoryAllocator();
};

}
