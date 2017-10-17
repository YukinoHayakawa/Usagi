#include "StackMemoryAllocator.hpp"

yuki::StackMemoryAllocator::StackMemoryAllocator(size_t memory_begin, size_t total_size)
    : mMemoryBegin { memory_begin }
    , mCurrentPos { memory_begin }
    , mTotalSize { total_size }
{
}

void yuki::StackMemoryAllocator::reset()
{
    mCurrentPos = 0;
}

size_t yuki::StackMemoryAllocator::allocate(size_t size, size_t alignment)
{
    const size_t aligned_address = roundUpUnsigned(mCurrentPos, alignment);
    const size_t next_pos = aligned_address + size;

    if(next_pos > mMemoryBegin + mTotalSize) throw MemoryAllocationException() << SizeInfo(size);

    mCurrentPos = next_pos;
    return aligned_address;
}

void yuki::StackMemoryAllocator::release(size_t offset)
{
}
