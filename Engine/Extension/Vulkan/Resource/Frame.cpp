#include <Usagi/Engine/Runtime/Exception.hpp>
#include <Usagi/Engine/Runtime/Memory/StackMemoryAllocator.hpp>

#include "Frame.hpp"
#include "Buffer.hpp"
#include "../Device/Device.hpp"
#include "../Workload/CommandPool.hpp"
#include "../Workload/CommandList.hpp"
#include "Image.hpp"

namespace yuki::vulkan
{

Frame::Frame(Device *device)
    : mDevice(device)
    // todo duplicated size info & customized/dynamic size
    , mFrameStackAllocator{ 1024 * 1024 * 128 /* 128MB */, device->_getDevice(), device->_getPhysicalDevice(), std::make_unique<StackMemoryAllocator>(0, 1024 * 1024 * 128) }
{
    mCommandPool.reset(dynamic_cast<CommandPool*>(device->createGraphicsCommandPool().release()));
    mCommandList.reset(dynamic_cast<CommandList*>(mCommandPool->createCommandList().release()));

    // todo correct stage
    vk::SemaphoreCreateInfo semaphore_create_info;
    mRenderingFinishedSemaphore = { device->_getDevice().createSemaphoreUnique(semaphore_create_info), vk::PipelineStageFlagBits::eBottomOfPipe };

    // create finished fence with signaled bit to avoid waiting for the first frame indefinitely
    vk::FenceCreateInfo fence_create_info;
    fence_create_info.setFlags(vk::FenceCreateFlagBits::eSignaled);
    mFrameFinishedFence = device->_getDevice().createFenceUnique(fence_create_info);
    mAliasFence.fence = mFrameFinishedFence.get();
}

void Frame::beginFrame(const std::vector<graphics::Image *> &attachments)
{
    // reset stack allocator
    mFrameStackAllocator.reset();

    mCommandList->_setAttachments(attachments);

    mDevice->_getDevice().waitForFences({ mFrameFinishedFence.get() }, true, UINT64_MAX);
    mDevice->_getDevice().resetFences({ mFrameFinishedFence.get() });
}

graphics::CommandList * Frame::getCommandList()
{
    return mCommandList.get();
}

graphics::ResourceAllocator * Frame::getResourceAllocator()
{
    // todo this is actually implemented in the frame controller, but leaving this returning null means there are problems with the interface design
    return nullptr;
}

graphics::Waitable * Frame::getRenderingFinishedSemaphore()
{
    return &mRenderingFinishedSemaphore;
}

graphics::Waitable * Frame::getRenderingFinishedFence()
{
    return &mAliasFence;
}

void Frame::endFrame()
{
}

MemoryAllocator * Frame::_getStackMemoryAllocator()
{
    return &mFrameStackAllocator;
}

}
