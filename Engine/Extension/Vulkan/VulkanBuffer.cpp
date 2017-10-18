#include "VulkanMemoryAllocator.hpp"
#include "VulkanBuffer.hpp"

// todo: evaluate the possibility of moving most method implementation into the base
yuki::VulkanBuffer::VulkanBuffer(size_t size, class GraphicsMemoryAllocator *allocator)
    : mAllocator(allocator)
{
    mAllocation = allocator->allocate(size);
}

void * yuki::VulkanBuffer::getMappedAddress()
{
    return mAllocator->getMappedAddress(mAllocation);
}

void yuki::VulkanBuffer::flush()
{
    mAllocator->flushRange(mAllocation, 0, mAllocation.size);
}

vk::Buffer yuki::VulkanBuffer::_getBuffer() const
{
    return dynamic_cast<VulkanMemoryAllocator*>(mAllocator->getActualAllocator(mAllocation))->_getBuffer();
}

size_t yuki::VulkanBuffer::_getOffset() const
{
    return mAllocation.offset;
}

void yuki::VulkanBuffer::reallocate()
{
    mAllocation = mAllocator->reallocate(mAllocation);
}
