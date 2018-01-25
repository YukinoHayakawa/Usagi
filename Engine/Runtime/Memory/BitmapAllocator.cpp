#include "BitmapAllocator.hpp"

#include <Usagi/Engine/Utility/BitHack.hpp>
#include <Usagi/Engine/Utility/Rounding.hpp>

yuki::memory::BitmapAllocator::BitmapAllocator(void *base, const size_t total_size,
    const size_t block_size, const size_t max_alignment)
    : mBase { static_cast<char*>(base) }
    , mTotalSize { total_size }
    , mBlockSize { block_size }
    , mMaxAlignment { max_alignment }
{
    if(!block_size)
        throw std::invalid_argument(
            "block size must be positive");
    if(total_size < block_size)
        throw std::invalid_argument(
            "total size cannot hold a single block");
    if(!utility::isPowerOfTwoOrZero(max_alignment))
        throw std::invalid_argument(
            "alignment must be power-of-two or zero");
    if(max_alignment)
    {
        if(reinterpret_cast<size_t>(mBase) % max_alignment)
            throw std::invalid_argument(
                "base address not aligned to maximum alignment");
        if(block_size % max_alignment)
            throw std::invalid_argument(
                "block size is not a multiple of maximum alignment");
    }

    mAllocation.reset(total_size / block_size);
}

void * yuki::memory::BitmapAllocator::allocate(const size_t num_bytes,
    const size_t alignment)
{
    if(num_bytes == 0)
        throw std::invalid_argument(
            "allocation size must be positive");
    if(alignment > mMaxAlignment)
        throw std::invalid_argument(
            "alignment request is greater than the maximum allowed");
    if(!utility::isPowerOfTwoOrZero(alignment))
        throw std::invalid_argument("alignment must be power-of-two or zero");

    const size_t alloc_unit = utility::calculateSpanningPages(num_bytes,
        mBlockSize);

    std::lock_guard<std::mutex> lock_guard(mBitmapLock);
    const auto allocation = mAllocation.allocate(alloc_unit);
    return mBase + allocation * mBlockSize;
}

void yuki::memory::BitmapAllocator::deallocate(void *pointer)
{
    const size_t offset = static_cast<char*>(pointer) - mBase;
    const size_t block = offset / mBlockSize;
    if(offset % mBlockSize != 0)
        throw std::invalid_argument(
            "the pointer does not point to the beginning of any block");

    std::lock_guard<std::mutex> lock_guard(mBitmapLock);
    mAllocation.deallocate(block);
}
