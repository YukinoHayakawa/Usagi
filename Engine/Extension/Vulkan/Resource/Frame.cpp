#include <Usagi/Engine/Runtime/Exception.hpp>

#include "Frame.hpp"
#include "../Device/Device.hpp"
#include "../Workload/CommandPool.hpp"
#include "../Workload/CommandList.hpp"
#include "Image.hpp"

namespace yuki::extension::vulkan
{
Frame::Frame(Device *device)
    : mDevice(device)
{
    mCommandPool.reset(
        dynamic_cast<CommandPool*>(device->createGraphicsCommandPool().release()));
    mCommandList.reset(
        dynamic_cast<CommandList*>(mCommandPool->createCommandList().release()));

    // todo correct stage
    vk::SemaphoreCreateInfo semaphore_create_info;
    mRenderingFinishedSemaphore = {
        device->device().createSemaphoreUnique(semaphore_create_info),
        vk::PipelineStageFlagBits::eBottomOfPipe
    };

    // create finished fence with signaled bit to avoid waiting for the first frame indefinitely
    vk::FenceCreateInfo fence_create_info;
    fence_create_info.setFlags(vk::FenceCreateFlagBits::eSignaled);
    mFrameFinishedFence = device->device().createFenceUnique(fence_create_info);
    mAliasFence.fence = mFrameFinishedFence.get();
}

void Frame::beginFrame(const std::vector<graphics::Image *> &attachments)
{
    mCommandList->_setAttachments(attachments);

    mDevice->device().waitForFences({ mFrameFinishedFence.get() }, true, UINT64_MAX);
    mDevice->device().resetFences({ mFrameFinishedFence.get() });
}

graphics::CommandList * Frame::getCommandList()
{
    return mCommandList.get();
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
}
