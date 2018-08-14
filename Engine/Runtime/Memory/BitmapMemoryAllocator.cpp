#include "BitmapMemoryAllocator.hpp"

#include <Usagi/Engine/Utility/BitHack.hpp>
#include <Usagi/Engine/Utility/Rounding.hpp>

namespace usagi
{
BitmapMemoryAllocator::BitmapMemoryAllocator(void *base,
    const std::size_t total_size,
    const std::size_t block_size, const std::size_t alignment)
    : mBase { static_cast<char*>(base) }
    , mTotalSize { total_size }
    , mBlockSize { block_size }
    , mAlignment { alignment }
{
    if(!block_size)
        throw std::invalid_argument(
            "block size must be positive");
    if(total_size < block_size)
        throw std::invalid_argument(
            "total size cannot hold a single block");
    if(!utility::isPowerOfTwoOrZero(mAlignment))
        throw std::invalid_argument(
            "alignment must be power-of-two or zero");
    if(mAlignment)
    {
        if(reinterpret_cast<std::size_t>(mBase) % mAlignment != 0)
            throw std::invalid_argument(
                "base address not aligned to alignment requirement");
        if(block_size % mAlignment != 0)
            throw std::invalid_argument(
                "block size is not a multiple of alignment requirement");
    }

    mAllocation.reset(total_size / block_size);
}

void * BitmapMemoryAllocator::allocate(const std::size_t num_bytes)
{
    if(num_bytes == 0)
        throw std::invalid_argument(
            "allocation size must be positive");
    //if(alignment > mMaxAlignment)
    //    throw std::invalid_argument(
    //        "alignment request is greater than the maximum allowed");
    //if(!utility::isPowerOfTwoOrZero(alignment))
    //    throw std::invalid_argument("alignment must be power-of-two or zero");

    const auto alloc_unit = utility::calculateSpanningPages(
        num_bytes, mBlockSize);

    std::size_t allocation;
    {
        std::lock_guard<std::mutex> lock_guard(mBitmapLock);
        allocation = mAllocation.allocate(alloc_unit);
    }

    return mBase + allocation * mBlockSize;
}

void BitmapMemoryAllocator::deallocate(void *pointer)
{
    const std::size_t offset = static_cast<char*>(pointer) - mBase;
    const std::size_t block = offset / mBlockSize;
    if(offset % mBlockSize != 0)
        throw std::invalid_argument(
            "the pointer does not point to the beginning of any block");

    std::lock_guard<std::mutex> lock_guard(mBitmapLock);
    mAllocation.deallocate(block);
}
}
