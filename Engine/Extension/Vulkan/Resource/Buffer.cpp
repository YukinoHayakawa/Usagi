#include "MemoryAllocator.hpp"
#include "Buffer.hpp"

namespace yuki::vulkan
{

// todo: evaluate the possibility of moving most method implementation into the base
Buffer::Buffer(size_t size, graphics::MemoryAllocator *allocator)
    : mAllocator(allocator)
{
    mAllocation = allocator->allocate(size);
}

void * Buffer::getMappedAddress()
{
    return mAllocator->getMappedAddress(mAllocation);
}

void Buffer::flush()
{
    mAllocator->flushRange(mAllocation, 0, mAllocation.size);
}

vk::Buffer Buffer::_getBuffer() const
{
    return dynamic_cast<MemoryAllocator*>(mAllocator->getActualAllocator(mAllocation))->_getBuffer();
}

size_t Buffer::_getOffset() const
{
    return mAllocation.offset;
}

void Buffer::reallocate()
{
    mAllocation = mAllocator->reallocate(mAllocation);
}

}
