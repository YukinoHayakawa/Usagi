#include "BitmapMemoryAllocator.hpp"

#include <algorithm>

#include <Usagi/Utility/Rounding.hpp>

namespace usagi
{
std::size_t BitmapMemoryAllocator::getAddressBlock(
    const std::size_t address) const
{
    assert(address >= mBase);
    const auto rel = address - mBase;
    const auto block = rel / mBlockSize;
    assert(block < mBitmap.size());
    return block;
}

BitmapMemoryAllocator::BitmapMemoryAllocator(void *base,
    const std::size_t total_size,
    const std::size_t block_size)
    : mBase { reinterpret_cast<std::size_t>(base) }
    , mTotalSize { total_size }
    , mBlockSize { block_size }
{
    static_assert(sizeof(char) == 1);

    if(!block_size)
        throw std::invalid_argument(
            "block size must be positive");
    if(total_size < block_size)
        throw std::invalid_argument(
            "total size cannot hold a single block");

    mBitmap = std::string(total_size / block_size, BLOCK_FREE);
}

std::size_t BitmapMemoryAllocator::usableSize() const
{
    return mBlockSize * std::count(mBitmap.begin(), mBitmap.end(), BLOCK_FREE);
}

void BitmapMemoryAllocator::markBlocksAllocated(
    std::string::iterator begin,
    std::string::iterator end)
{
    *begin = BLOCK_USED_BEGIN;
    std::fill(++begin, end, BLOCK_USED);
}

void * BitmapMemoryAllocator::allocate(
    const std::size_t num_bytes, const std::size_t alignment)
{
    if(num_bytes == 0)
        throw std::invalid_argument("allocation size must be greater than 0");

    std::lock_guard<std::mutex> lock_guard(mBitmapLock);

    if(alignment == 0)
    {
        const auto alloc_unit = utility::calculateSpanningPages(
            num_bytes, mBlockSize);

        const auto first_free_block = mBitmap.find(std::string(
            alloc_unit, BLOCK_FREE));
        if(first_free_block == std::string::npos)
            throw std::bad_alloc();
        markBlocksAllocated(
            mBitmap.begin() + first_free_block,
            mBitmap.begin() + first_free_block + alloc_unit
        );
        return reinterpret_cast<void*>(mBase + first_free_block * mBlockSize);
    }

    auto first_addr = utility::roundUpUnsigned(mBase, alignment);
    const auto end_addr = mBase + mBlockSize * mBitmap.size();
    while(true)
    {
        if(first_addr >= end_addr)
            throw std::bad_alloc();

        const auto last_addr = first_addr + num_bytes - 1;
        const auto begin_block = mBitmap.begin() + getAddressBlock(first_addr);
        const auto end_block = mBitmap.begin() + getAddressBlock(last_addr) + 1;
        if(std::all_of(
            begin_block, end_block,
            [](const char c) { return c == BLOCK_FREE; }))
        {
            markBlocksAllocated(begin_block, end_block);
            return reinterpret_cast<void*>(first_addr);
        }

        first_addr += alignment;
    }
}

void BitmapMemoryAllocator::deallocate(void *pointer)
{
    std::lock_guard<std::mutex> lock_guard(mBitmapLock);

    const auto block = getAddressBlock(reinterpret_cast<std::size_t>(pointer));
    auto iter = mBitmap.begin() + block;
    assert(*iter == BLOCK_USED_BEGIN);
    *iter = BLOCK_FREE;
    while(*++iter == BLOCK_USED)
        *iter = BLOCK_FREE;
}
}
