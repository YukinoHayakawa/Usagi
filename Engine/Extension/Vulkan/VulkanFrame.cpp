#include <Usagi/Engine/Runtime/Exception.hpp>
#include <Usagi/Engine/Runtime/Memory/StackMemoryAllocator.hpp>

#include "VulkanFrame.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanGraphicsDevice.hpp"
#include "VulkanGraphicsCommandPool.hpp"
#include "VulkanGraphicsCommandList.hpp"
#include "VulkanImage.hpp"

yuki::VulkanFrame::VulkanFrame(VulkanGraphicsDevice *device)
    : mDevice(device)
    // todo duplicated size info & customized/dynamic size
    , mFrameStackAllocator { 1024 * 1024 * 128 /* 128MB */, device->_getDevice(), device->_getPhysicalDevice(), std::make_unique<StackMemoryAllocator>(0, 1024 * 1024 * 128) }
{
    mCommandPool.reset(dynamic_cast<VulkanGraphicsCommandPool*>(device->createGraphicsCommandPool().release()));
    mCommandList.reset(dynamic_cast<VulkanGraphicsCommandList*>(mCommandPool->createCommandList().release()));
    
    // todo correct stage
    vk::SemaphoreCreateInfo semaphore_create_info;
    mRenderingFinishedSemaphore = { device->_getDevice().createSemaphoreUnique(semaphore_create_info), vk::PipelineStageFlagBits::eBottomOfPipe };

    // create finished fence with signaled bit to avoid waiting for the first frame indefinitely
    vk::FenceCreateInfo fence_create_info;
    fence_create_info.setFlags(vk::FenceCreateFlagBits::eSignaled);
    mFrameFinishedFence = device->_getDevice().createFenceUnique(fence_create_info);
}

void yuki::VulkanFrame::beginFrame(const std::vector<GraphicsImage *> &attachments)
{
    // reset stack allocator
    mFrameStackAllocator.reset();

    mCommandList->_setAttachments(attachments);

    mDevice->_getDevice().waitForFences({ mFrameFinishedFence.get() }, true, UINT64_MAX);
    mDevice->_getDevice().resetFences({ mFrameFinishedFence.get() });
}

yuki::GraphicsCommandList * yuki::VulkanFrame::getCommandList()
{
    return mCommandList.get();
}

yuki::GraphicsResourceAllocator * yuki::VulkanFrame::getResourceAllocator()
{
    // todo this is actually implemented in the frame controller, but leaving this returning null means there are problems with the interface design
    return nullptr;
}

yuki::GraphicsSemaphore * yuki::VulkanFrame::getRenderingFinishedSemaphore()
{
    return &mRenderingFinishedSemaphore;
}

void yuki::VulkanFrame::endFrame()
{
}

yuki::VulkanMemoryAllocator * yuki::VulkanFrame::_getStackMemoryAllocator()
{
    return &mFrameStackAllocator;
}
