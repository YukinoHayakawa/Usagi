#pragma once

#include <mutex>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

#include "detail/MemoryBitmap.hpp"

namespace usagi
{
/**
 * \brief A thread-safe bitmap allocator for managing remote memory.
 */
class BitmapMemoryAllocator : Noncopyable
{
    char *const mBase = nullptr;
    const std::size_t mTotalSize = 0;
    const std::size_t mBlockSize = 0;
    // alignment currently has no use except for checking block size
    const std::size_t mAlignment = 0;
    detail::MemoryBitmap mAllocation;
    std::mutex mBitmapLock;

public:
    /**
     * \brief
     * \param base The starting address of the memory region. It must be
     * aligned to max_alignment, and can be nullptr to allocate based on
     * offsets.
     * \param total_size Total usable size. Must be greater than block_size.
     * If it is not a multiple of block_size, the region at the end not
     * big enough to hold a block will not be used.
     * \param block_size A positive size of minimum allocation unit. Must
     * be multiple of max_alignment if which is non-zero.
     * \param alignment The minimum alignment requirement. Must be power
     * of two. It can be zero if no alignment is needed.
     */
    BitmapMemoryAllocator(
        void *base,
        std::size_t total_size,
        std::size_t block_size,
        std::size_t alignment);

    std::size_t managedSize() const { return mTotalSize; }

    std::size_t usableSize() const
    {
        return mBlockSize * mAllocation.blockCount();
    }

    std::size_t usedSize() const { return managedSize() - usableSize(); }

    void * allocate(std::size_t num_bytes);
    void deallocate(void *pointer);
};
}
