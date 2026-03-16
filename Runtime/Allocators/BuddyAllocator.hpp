#pragma once

#include <Usagi/Runtime/Allocators/Concepts/ReallocatableAllocator.hpp>
#include <Usagi/Runtime/Storage/Views/MemoryView.hpp>

#include "AllocatorCommon.hpp"

namespace usagi::runtime::allocators
{
/**
 * \brief Intrusive block header for the Offset-based Buddy Allocator.
 *
 * Shio:
 * Classic Buddy allocators use `void* next`. We use `uint32_t offset` to
 * maintain absolute position independence within the MemoryView.
 */
struct BuddyBlockHeader
{
    std::uint32_t size_order; // The power-of-2 order of this block
    std::uint32_t is_free;    // 1 if free, 0 if allocated
    std::uint32_t next_free;  // Offset to the next free block of the same order
    std::uint32_t prev_free;  // Offset to the previous free block
};

/**
 * \brief The control block mapped to the start of the memory region.
 */
struct BuddyHeapHeader
{
    static constexpr std::uint64_t EXPECTED_MAGIC =
        0x4255'4444'5948'5000; // "BUDDYHP\0"

    std::uint64_t magic;
    std::uint32_t min_order;
    std::uint32_t max_order;
    std::uint32_t total_size;

    // The starting offset of the first allocatable block.
    // Crucially, this must be a strict power of 2 relative to the physical
    // start, otherwise the `rel_offset ^ size` math for coalescing will fail!
    std::uint32_t payload_base_offset;

    // Array of free list head offsets for each size order
    // order 0 = min_order, order N = max_order
    std::uint32_t free_lists[32];
};

/**
 * \brief A position-independent, Buddy Allocator for variable-sized dynamic
 * allocations.
 *
 * Shio:
 * Splits memory into power-of-two sized blocks.
 * To find the "buddy" of a block, you simply XOR its relative
 * offset with its size. If both buddies are free, they merge into the next
 * order up.
 *
 * **Use Cases:**
 * - **Medium-to-Large Dynamic Arrays:** Excellent for dynamic buffers that grow
 * and shrink in power-of-two increments (e.g., hash tables, vector capacities).
 * - **Fast Coalescing:** The mathematical XOR neighbor resolution makes merging
 * freed blocks extremely fast compared to traversing linked lists.
 * - **Fragmentation Trade-off:** While fast, it suffers from internal
 * fragmentation (e.g., requesting 33KB will consume a full 64KB block). It
 * should not be used for highly variable or exact-size string allocations.
 */
class BuddyAllocator
{
    storage::MemoryView mMemory;

    [[nodiscard]]
    BuddyHeapHeader *header() noexcept
    {
        return mMemory.cast_view<BuddyHeapHeader>();
    }

    [[nodiscard]]
    BuddyBlockHeader *get_block(std::uint32_t offset);

    void list_remove(std::uint32_t offset);
    void list_insert(std::uint32_t offset);

public:
    static constexpr std::uint8_t SIGNATURE =
        allocator_signature(AllocatorType::Buddy);

    BuddyAllocator(
        storage::MemoryView memory, std::uint32_t min_alloc_size,
        std::uint32_t total_size, bool force_format = false);

    [[nodiscard]]
    MemoryHandle allocate(
        std::uint64_t size, storage::StorageAlignment alignment);

    void deallocate(MemoryHandle handle);

    [[nodiscard]]
    MemoryHandle reallocate(
        MemoryHandle handle, std::uint64_t new_size,
        storage::StorageAlignment alignment);

    [[nodiscard]]
    void *resolve(MemoryHandle handle) noexcept;

    [[nodiscard]]
    const storage::MemoryView &view() const noexcept
    {
        return mMemory;
    }
};

static_assert(ReallocatableAllocator<BuddyAllocator>);
} // namespace usagi::runtime::allocators
