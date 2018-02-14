#pragma once

#include <vector>

#include <Usagi/Engine/Runtime/Graphics/Resource/FrameController.hpp>

namespace yuki::extension::vulkan
{

class Frame;

class FrameController : public graphics::FrameController
{
    class Device *mDevice;
    std::vector<std::unique_ptr<vulkan::Frame>> mFrameChain; // must have at least one element

    vulkan::Frame * _getCurrentFrame();

public:
    FrameController(Device *device, size_t num_frames);

    size_t getFrameCount() const override final;

    void beginFrame(const std::vector<class graphics::Image *> &attachments) override;
    graphics::CommandList * getCommandList() override;
    graphics::Waitable * getRenderingFinishedSemaphore() override;
    graphics::Waitable * getRenderingFinishedFence() override;
    void endFrame() override;
};

}
