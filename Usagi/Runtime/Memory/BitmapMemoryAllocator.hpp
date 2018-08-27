#pragma once

#include <mutex>

#include <Usagi/Utility/Noncopyable.hpp>

namespace usagi
{
/**
 * \brief A thread-safe bitmap allocator for managing remote memory.
 */
class BitmapMemoryAllocator : Noncopyable
{
    enum Block : char
    {
        BLOCK_FREE = 0,
        BLOCK_USED = 1,
        BLOCK_USED_BEGIN = 2,
    };

    const std::size_t mBase = 0;
    const std::size_t mTotalSize = 0;
    const std::size_t mBlockSize = 0;
    std::string mBitmap;
    std::mutex mBitmapLock;

    std::size_t getAddressBlock(std::size_t address) const;
    void markBlocksAllocated(
        std::string::iterator begin,
        std::string::iterator end);

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
     */
    BitmapMemoryAllocator(
        void *base,
        std::size_t total_size,
        std::size_t block_size);

    std::size_t managedSize() const { return mTotalSize; }
    std::size_t usableSize() const;
    std::size_t usedSize() const { return managedSize() - usableSize(); }

    void * allocate(std::size_t num_bytes, std::size_t alignment = 0);
    void deallocate(void *pointer);
};
}
