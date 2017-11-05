#pragma once

#include <vector>

#include <Usagi/Engine/Runtime/Graphics/Resource/FrameController.hpp>

namespace yuki::vulkan
{

class Frame;

class FrameController
    : public graphics::FrameController
    , public graphics::MemoryAllocator
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

    std::unique_ptr<graphics::Buffer> createDynamicBuffer(size_t size) override;

private:
    Allocation allocate(size_t size) override;
    void release(const Allocation &alloc_info) override;
    Allocation reallocate(const Allocation &alloc_info) override;
    void reset() override;
    void * getMappedAddress(const Allocation &allocation) const override;
    void flushRange(const Allocation &allocation, size_t offset, size_t size) override;
    graphics::MemoryAllocator * getActualAllocator(const Allocation &allocation) override;
};

}
