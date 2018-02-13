#pragma once

#include <Usagi/Engine/Runtime/Graphics/Resource/Frame.hpp>
#include "../Device/Semaphore.hpp"
#include "../Device/Fence.hpp"

namespace yuki::extension::vulkan
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

public:
    explicit Frame(Device *device);

    void beginFrame(const std::vector<graphics::Image *> &attachments) override;

    graphics::CommandList * getCommandList() override;
    graphics::Waitable * getRenderingFinishedSemaphore() override;
    graphics::Waitable * getRenderingFinishedFence() override;

    void endFrame() override;
};

}
