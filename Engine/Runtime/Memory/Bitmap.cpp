#include <cassert>

#include <Usagi/Engine/Utility/CyclicContainerAdaptor.hpp>

#include "Bitmap.hpp"

yuki::memory::Bitmap::Bitmap(size_t num_blocks)
    : mBitmap(num_blocks, Block::FREE)
{
}

size_t yuki::memory::Bitmap::allocate(size_t num_blocks, size_t start_block)
{
    assert(start_block < mBitmap.size());

    if(num_blocks == 0)
        throw std::invalid_argument("allocation size must be positive");

    // forbid allocating from the middle of a free sequence
    auto start_iter = mBitmap.begin() + start_block;
    if(start_block != 0 && *(start_iter - 1) != Block::USED)
        start_iter = mBitmap.begin();

    utility::CyclicContainerAdaptor<decltype(mBitmap)> cyclic_bitmap(mBitmap,
        start_iter, 1);

    size_t free_block_count = 0;
    auto iter = cyclic_bitmap.begin();
    auto alloc_begin = iter.base();
    
    auto reset_alloc_finder = [&]() { free_block_count = 0; alloc_begin = iter.base(); };

    reset_alloc_finder();
    iter.setWrapCallback([&](int direction) { reset_alloc_finder(); });
    for(; iter != cyclic_bitmap.end();)
    {
        if(*iter != Block::FREE)
        {
            ++iter;
            reset_alloc_finder();
        }
        else
        {
            ++free_block_count;
            if(free_block_count == num_blocks) break;
            ++iter;
        }
    }

    if(free_block_count < num_blocks)
        throw std::bad_alloc();

    const auto alloc_begin_index = alloc_begin - mBitmap.begin();
    *alloc_begin = Block::USED_BEGIN;
    for(size_t i = 1; i < num_blocks; ++i)
    {
        *++alloc_begin = Block::USED;
    }

    return alloc_begin_index;
}

void yuki::memory::Bitmap::deallocate(const size_t start_block)
{
    assert(start_block < mBitmap.size());

    auto iter = mBitmap.begin() + start_block;
    if(*iter != Block::USED_BEGIN)
        throw std::invalid_argument("argument not representing an allocation beginning");

    while(true)
    {
        *iter = Block::FREE;
        ++iter;
        if(iter == mBitmap.end() || *iter != Block::USED) break;
    }
}
