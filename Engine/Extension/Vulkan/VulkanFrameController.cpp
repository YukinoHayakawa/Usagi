#include <Usagi/Engine/Runtime/Exception.hpp>

#include "VulkanFrameController.hpp"
#include "VulkanFrame.hpp"
#include "VulkanBuffer.hpp"

std::unique_ptr<yuki::GraphicsBuffer> yuki::VulkanFrameController::createDynamicBuffer(size_t size)
{
    return std::make_unique<VulkanBuffer>(size, this);
}

yuki::GraphicsMemoryAllocator::Allocation yuki::VulkanFrameController::allocate(size_t size)
{
    auto alloc = _getCurrentFrame()->_getStackMemoryAllocator()->allocate(size);
    alloc.pool_id = mCurrentFrame;
    return alloc;
}

void yuki::VulkanFrameController::release(const Allocation &alloc_info)
{
    // todo
}

yuki::GraphicsMemoryAllocator::Allocation yuki::VulkanFrameController::reallocate(const Allocation &alloc_info)
{
    return allocate(alloc_info.size);
}

void yuki::VulkanFrameController::reset()
{

}

void * yuki::VulkanFrameController::getMappedAddress(const Allocation &allocation) const
{
    return mFrameChain[allocation.pool_id]->_getStackMemoryAllocator()->getMappedAddress(allocation);
}

void yuki::VulkanFrameController::flushRange(const Allocation &allocation, size_t offset, size_t size)
{
    return getActualAllocator(allocation)->flushRange(allocation, offset, size);
}

yuki::GraphicsMemoryAllocator * yuki::VulkanFrameController::getActualAllocator(const yuki::GraphicsMemoryAllocator::Allocation &allocation)
{
    return mFrameChain[allocation.pool_id]->_getStackMemoryAllocator();
}

yuki::VulkanFrame * yuki::VulkanFrameController::_getCurrentFrame()
{
    return mFrameChain[mCurrentFrame].get();
}

yuki::VulkanFrameController::VulkanFrameController(VulkanGraphicsDevice *device, size_t num_frames)
    : mDevice { device }
{
    if(num_frames == 0) throw InvalidArgumentException();
    mFrameChain.reserve(num_frames);
    for(size_t i = 0; i < num_frames; ++i)
    {
        mFrameChain.emplace_back(std::make_unique<VulkanFrame>(mDevice));
    }
}

size_t yuki::VulkanFrameController::getFrameCount() const
{
    return mFrameChain.size();
}

void yuki::VulkanFrameController::beginFrame(const std::vector<class GraphicsImage *> &attachments)
{
    _getCurrentFrame()->beginFrame(attachments);
}

yuki::GraphicsCommandList * yuki::VulkanFrameController::getCommandList()
{
    return _getCurrentFrame()->getCommandList();
}

yuki::GraphicsSemaphore * yuki::VulkanFrameController::getRenderingFinishedSemaphore()
{
    return _getCurrentFrame()->getRenderingFinishedSemaphore();
}

void yuki::VulkanFrameController::endFrame()
{
    _getCurrentFrame()->endFrame();
    _swapFrame();
}
