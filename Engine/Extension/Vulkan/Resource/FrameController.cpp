#include <Usagi/Engine/Runtime/Exception.hpp>

#include "FrameController.hpp"
#include "Frame.hpp"
#include "Buffer.hpp"

namespace yuki::vulkan
{

std::unique_ptr<graphics::Buffer> FrameController::createDynamicBuffer(size_t size)
{
    return std::make_unique<Buffer>(size, this);
}

graphics::MemoryAllocator::Allocation FrameController::allocate(size_t size)
{
    auto alloc = _getCurrentFrame()->_getStackMemoryAllocator()->allocate(size);
    alloc.pool_id = mCurrentFrame;
    return alloc;
}

void FrameController::release(const Allocation &alloc_info)
{
    // todo
}

graphics::MemoryAllocator::Allocation FrameController::reallocate(const Allocation &alloc_info)
{
    return allocate(alloc_info.size);
}

void FrameController::reset()
{

}

void * FrameController::getMappedAddress(const Allocation &allocation) const
{
    return mFrameChain[allocation.pool_id]->_getStackMemoryAllocator()->getMappedAddress(allocation);
}

void FrameController::flushRange(const Allocation &allocation, size_t offset, size_t size)
{
    return getActualAllocator(allocation)->flushRange(allocation, offset, size);
}

graphics::MemoryAllocator * FrameController::getActualAllocator(const graphics::MemoryAllocator::Allocation &allocation)
{
    return mFrameChain[allocation.pool_id]->_getStackMemoryAllocator();
}

Frame * FrameController::_getCurrentFrame()
{
    return mFrameChain[mCurrentFrame].get();
}

FrameController::FrameController(Device *device, size_t num_frames)
    : mDevice { device }
{
    if(num_frames == 0) throw InvalidArgumentException();
    mFrameChain.reserve(num_frames);
    for(size_t i = 0; i < num_frames; ++i)
    {
        mFrameChain.emplace_back(std::make_unique<vulkan::Frame>(mDevice));
    }
}

size_t FrameController::getFrameCount() const
{
    return mFrameChain.size();
}

void FrameController::beginFrame(const std::vector<class graphics::Image *> &attachments)
{
    _getCurrentFrame()->beginFrame(attachments);
}

graphics::CommandList * FrameController::getCommandList()
{
    return _getCurrentFrame()->getCommandList();
}

graphics::Waitable * FrameController::getRenderingFinishedSemaphore()
{
    return _getCurrentFrame()->getRenderingFinishedSemaphore();
}

graphics::Waitable * FrameController::getRenderingFinishedFence()
{
    return _getCurrentFrame()->getRenderingFinishedFence();
}

void FrameController::endFrame()
{
    _getCurrentFrame()->endFrame();
    _swapFrame();
}

}
