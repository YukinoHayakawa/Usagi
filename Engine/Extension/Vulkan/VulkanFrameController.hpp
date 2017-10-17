#pragma once

#include <vector>

#include <Usagi/Engine/Runtime/Graphics/FrameController.hpp>

namespace yuki
{

class VulkanFrameController : public FrameController, GraphicsMemoryAllocator
{
    class VulkanGraphicsDevice *mDevice;
    std::vector<std::unique_ptr<class VulkanFrame>> mFrameChain; // must have at least one element

    VulkanFrame * _getCurrentFrame();

public:
    VulkanFrameController(VulkanGraphicsDevice *device, size_t num_frames);

    size_t getFrameCount() const override final;

    void beginFrame(const std::vector<class GraphicsImage *> &attachments) override;
    GraphicsCommandList * getCommandList() override;
    GraphicsSemaphore * getRenderingFinishedSemaphore() override;
    void endFrame() override;

    std::unique_ptr<GraphicsBuffer> createDynamicBuffer(size_t size) override;

private:
    Allocation allocate(size_t size) override;
    void release(const Allocation &alloc_info) override;
    Allocation reallocate(const Allocation &alloc_info) override;
    void reset() override;
    void * getMappedAddress(const Allocation &allocation) const override;
    void flushRange(const Allocation &allocation, size_t offset, size_t size) override;
    GraphicsMemoryAllocator * getActualAllocator(const Allocation &allocation) override;
};

}
