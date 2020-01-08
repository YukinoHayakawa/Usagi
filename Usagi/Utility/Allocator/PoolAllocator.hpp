﻿#pragma once

#include <vector>
#include <cassert>
#include <type_traits>
#include <utility>

#include <boost/iterator/filter_iterator.hpp>

namespace usagi
{
/**
 * \brief An object pool. The objects must be accessed via indices.
 * TODO: lock-free
 * todo: dump & load from file
 * todo: allow using memory-mapped file in place of the vector storage
 * \tparam T
 */
template <typename T>
class PoolAllocator
{
    constexpr static std::size_t MAGIC_CHECK = 0xDEADCELL;
    constexpr static std::size_t INVALID_BLOCK = -1;

    struct FreeBlock
    {
        std::size_t next = INVALID_BLOCK;
        std::size_t magic = MAGIC_CHECK;
    };

    union Block
    {
        FreeBlock free_block;
        T data;

        bool isFreeBlock() const
        {
            return free_block.magic == MAGIC_CHECK;
        }

        Block()
        {
            new (&free_block) FreeBlock();
        }

        ~Block()
        {
            if(!isFreeBlock())
                data.~T();
        }

        Block(Block &&other) noexcept
        {
            if(other.isFreeBlock())
            {
                new (&free_block) FreeBlock(other.free_block);
            }
            else
            {
                new (&data) T(other.data);
                other.data.~T();
                new (&other.free_block) FreeBlock();
            }
        }
    };

    std::vector<Block> mStorage;
    std::size_t mFreeListHead = INVALID_BLOCK;

    FreeBlock & castToFreeBlock(const std::size_t index)
    {
        return mStorage[index].free_block;
    }

    FreeBlock & freeBlock(const std::size_t index)
    {
        auto &fb = castToFreeBlock(index);
        assert(fb.magic == MAGIC_CHECK);
        return fb;
    }

    FreeBlock & headFreeBlock()
    {
        assert(!mStorage.empty());
        assert(mFreeListHead != INVALID_BLOCK);
        return freeBlock(mFreeListHead);
    }

    FreeBlock & resetToFreeBlock(const std::size_t index)
    {
        auto &b = mStorage[index];
        assert(b.free_block.magic != MAGIC_CHECK);
        b.data.~T();
        b.free_block = FreeBlock { };
        return b.free_block;
    }

    static bool isAllocatedBlock(const Block &b)
    {
        return b.free_block.magic != MAGIC_CHECK;
    }

public:
    T & block(const std::size_t index)
    {
        auto &b = mStorage[index];
        assert(b.free_block.magic != MAGIC_CHECK);
        return b.data;
    }

    template <typename... Args>
    std::size_t allocate(Args &&...args)
    {
        Block *block = nullptr;
        std::size_t idx = -1;
        if(mFreeListHead != INVALID_BLOCK)
        {
            idx = mFreeListHead;
            block = &mStorage[mFreeListHead];
            mFreeListHead = headFreeBlock().next;
        }
        else
        {
            idx = mStorage.size();
            mStorage.emplace_back();
            block = &mStorage.back();
        }
        // Prevent allocated block from appearing to be free
        block->free_block.magic = 0;
        new (&block->data) T(std::forward<Args>(args)...);
        return idx;
    }

    void deallocate(const std::size_t index)
    {
        auto &fp = resetToFreeBlock(index);
        if(mFreeListHead == INVALID_BLOCK)
        {
            mFreeListHead = index;
        }
        else
        {
            fp.next = mFreeListHead;
            mFreeListHead = index;
        }
    }

    std::size_t numBlocks() const
    {
        return mStorage.size();
    }

    std::size_t capacity() const
    {
        return mStorage.capacity();
    }

    static constexpr auto ALLOCATED_BLOCK_PREDICATE = [](auto &&block) {
        // DO NOT directly pass the function pointer to isAllocatedBlock
        // to filter_iterator because doing so will prevent inlining.
        return isAllocatedBlock(std::forward<decltype(block)>(block));
    };

    auto begin()
    {
        return boost::make_filter_iterator(
            ALLOCATED_BLOCK_PREDICATE,
            mStorage.begin(), mStorage.end()
        );
    }

    auto end()
    {
        return boost::make_filter_iterator(
            ALLOCATED_BLOCK_PREDICATE,
            mStorage.end(), mStorage.end()
        );
    }

    // using IteratorT = boost::filter_iterator<
    //     decltype(ALLOCATED_BLOCK_PREDICATE),
    //     typename decltype(mStorage)::iterator
    // >;

    using IteratorT = decltype(
        boost::make_filter_iterator(
            ALLOCATED_BLOCK_PREDICATE,
            mStorage.end(), mStorage.end()
        )
    );
};
}
