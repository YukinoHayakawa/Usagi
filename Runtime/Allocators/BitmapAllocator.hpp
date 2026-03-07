#pragma once

#include <Usagi/Runtime/Allocators/Concepts/FixedSizeAllocator.hpp>

namespace usagi::runtime::allocators
{
/**
 * \brief The header mapped at the base of the bitmap allocator's memory region.
 */
struct BitmapHeapHeader
{
    static constexpr std::uint64_t EXPECTED_MAGIC =
        0x5553'4147'4950'4147; // "USAGIPAG"

    std::uint64_t magic;
    std::uint32_t block_size;
    std::uint32_t block_alignment;
    std::uint32_t max_blocks;

    // The index of the first block that can be allocated (skipping the header's
    // own blocks)
    std::uint32_t first_data_block;
    std::uint32_t active_blocks;

    // Flexible array member. 1 bit = 1 block. 1 = Free, 0 = Used.
    std::uint64_t free_mask[];
};

/**
 * \brief A Position-Independent, Bitset-based Fixed Size Allocator.
 *
 * Shio:
 * This allocator divides the MemoryView into fixed-size blocks and returns
 * `MemoryHandle`s. Because it uses dense bitmasks (`_tzcnt_u64`), allocation is
 * strictly O(1), highly deterministic, and completely lock-free if synchronized
 * externally.
 *
 * **Use Cases:**
 * - **Entity ID Generation & Page Slicing:** Perfect for managing large swaths
 * of identical blocks, such as doling out 64KB pages to the `HiveAllocator`.
 * - **Pooling:** Ideal for fixed-size object pools where fragmentation isn't an
 * issue because every slot is identical.
 * - **Deterministic Scheduling:** Because its runtime performance is
 * mathematically constant (no searching or tree traversal), it guarantees zero
 * jitter during frame execution.
 */
class BitmapAllocator
{
    MemoryView mMemory;

    [[nodiscard]]
    BitmapHeapHeader *header() const noexcept
    {
        return reinterpret_cast<BitmapHeapHeader *>(mMemory.base_view());
    }

public:
    static constexpr std::uint8_t SIGNATURE = 1;

    BitmapAllocator(MemoryView    memory,
        std::uint32_t             block_size,
        storage::StorageAlignment alignment,
        std::uint32_t             max_blocks,
        bool                      force_format = false);

    [[nodiscard]]
    MemoryHandle allocate();
    void deallocate(MemoryHandle handle);

    [[nodiscard]]
    void *resolve(MemoryHandle handle) const noexcept;

    [[nodiscard]]
    std::uint64_t block_size() const noexcept;
    [[nodiscard]]
    storage::StorageAlignment alignment() const noexcept;

    [[nodiscard]]
    const MemoryView &view() const noexcept
    {
        return mMemory;
    }
};

static_assert(FixedSizeAllocator<BitmapAllocator>);
} // namespace usagi::runtime::allocators
