#pragma once

#include <Usagi/Engine/Runtime/Graphics/Resource/Frame.hpp>
#include <Usagi/Engine/Runtime/Graphics/Resource/ResourceAllocator.hpp>
#include "MemoryAllocator.hpp"
#include "../Device/Semaphore.hpp"
#include "../Device/Fence.hpp"

namespace yuki::vulkan
{

class Frame : public graphics::Frame
{
    class Device *mDevice;

    std::unique_ptr<class CommandPool> mCommandPool;
    std::unique_ptr<class CommandList> mCommandList;

    vk::Framebuffer mFramebuffer;
    Semaphore mRenderingFinishedSemaphore;
    vk::UniqueFence mFrameFinishedFence;
    Fence mAliasFence;
    MemoryAllocator mFrameStackAllocator; // reset on every frame

public:
    explicit Frame(Device *device);

    void beginFrame(const std::vector<graphics::Image *> &attachments) override;

    graphics::CommandList * getCommandList() override;
    graphics::ResourceAllocator * getResourceAllocator() override;
    graphics::Waitable * getRenderingFinishedSemaphore() override;
    graphics::Waitable * getRenderingFinishedFence() override;

    void endFrame() override;

    MemoryAllocator *_getStackMemoryAllocator();
};

}
