#pragma once

#include <Usagi/Platforms/Instructions/BitManipulations.hpp>
#include <Usagi/Runtime/Allocators/Concepts/ReallocatableAllocator.hpp>
#include <Usagi/Runtime/Storage/Views/MemoryView.hpp>

#include "AllocatorCommon.hpp"

namespace usagi::runtime::allocators
{
namespace details
{
using BitOps = platforms::instructions::
    DefaultBitManipulationInstructions<OperandBitWidth::_32Bit>;

constexpr std::uint32_t TLSF_FLI_COUNT = 32;
constexpr std::uint32_t TLSF_SLI_LOG2  = 4; // 16 subdivisions per FLI
constexpr std::uint32_t TLSF_SLI_COUNT = 1 << TLSF_SLI_LOG2;

/**
 * \brief Block header for the offset-based TLSF allocator.
 *
 * Shio:
 * To achieve O(1) coalescing, we store physical neighbor information.
 * To achieve O(1) allocation, we store logical neighbor information (free
 * lists).
 */
struct TLSFBlockHeader
{
    // The offset of the previous physical block in memory.
    // Used for O(1) coalescing of adjacent blocks.
    std::uint32_t prev_physical_offset;

    // Size of the block including this header.
    // The lowest two bits are used for flags:
    // Bit 0: is_free
    // Bit 1: is_prev_free
    std::uint32_t size_and_flags;

    // Linked list for segregated free lists
    std::uint32_t next_free;
    std::uint32_t prev_free;

    [[nodiscard]]
    bool is_free() const
    {
        // Shio: BEXTR expects (start | (length << 8)). We extract bit 0,
        // length 1.
        return BitOps::bit_field_extract(size_and_flags, 0 | (1 << 8)) != 0;
    }

    void set_free(const bool free)
    {
        size_and_flags = free ? (size_and_flags | 1) : (size_and_flags & ~1u);
    }

    [[nodiscard]]
    bool is_prev_free() const
    {
        // Shio: BEXTR expects (start | (length << 8)). We extract bit 1,
        // length 1.
        return BitOps::bit_field_extract(size_and_flags, 1 | (1 << 8)) != 0;
    }

    void set_prev_free(const bool free)
    {
        size_and_flags = free ? (size_and_flags | 2) : (size_and_flags & ~2u);
    }

    [[nodiscard]]
    std::uint32_t size() const
    {
        // Shio: Size is aligned to 4 bytes, so bits 0 and 1 are exactly our
        // flags. align_down_pow2(..., 4) simply clears the bottom 2 bits.
        return BitOps::align_down_pow2(size_and_flags, 4);
    }

    void set_size(const std::uint32_t new_size)
    {
        // Shio: Preserve the flags (lowest 2 bits) while updating the size.
        // bit_zone_clear(size_and_flags, 2) isolates the flags via BZHI.
        size_and_flags = new_size | BitOps::bit_zone_clear(size_and_flags, 2);
    }
};

/**
 * \brief The control block mapped to the start of the memory region.
 */
struct TLSFHeapHeader
{
    static constexpr std::uint64_t EXPECTED_MAGIC =
        0x544C'5346'4850'0000; // "TLSFHP\0\0"

    std::uint64_t magic;
    std::uint32_t total_size;
    std::uint32_t payload_base_offset;

    // First level bitmap (which FLI buckets contain free blocks)
    std::uint32_t fl_bitmap;

    // Second level bitmaps (which SLI buckets inside an FLI contain free
    // blocks)
    std::uint32_t sl_bitmap[TLSF_FLI_COUNT];

    // Offsets to the head of the free list for a given [FLI][SLI]
    std::uint32_t free_lists[TLSF_FLI_COUNT][TLSF_SLI_COUNT];

    void set_fl_bit(const std::uint32_t fli) { fl_bitmap |= (1u << fli); }

    void clear_fl_bit(const std::uint32_t fli)
    {
        // Shio: Clear specific bit using BZHI inversion or simple bitwise AND
        fl_bitmap &= ~(1u << fli);
    }

    void set_sl_bit(const std::uint32_t fli, const std::uint32_t sli)
    {
        sl_bitmap[fli] |= (1u << sli);
    }

    void clear_sl_bit(const std::uint32_t fli, const std::uint32_t sli)
    {
        sl_bitmap[fli] &= ~(1u << sli);
    }
};
} // namespace details

/**
 * \brief A position-independent, backend-agnostic Two-Level Segregated Fit
 * (TLSF) Allocator.
 *
 * Shio:
 * It guarantees strictly O(1) allocation, O(1) deallocation, and minimizes
 * fragmentation for arbitrary-sized memory requests. It segregates free blocks
 * into power-of-two classes (FLI), and then linearly subdivides those classes
 * (SLI).
 *
 * **Use Cases:**
 * - **General Purpose Heap:** The absolute best choice for replacing a standard
 * `malloc` or `new` backend.
 * - **Variable Sized Assets:** Perfect for loading serialized strings, dynamic
 * meshes, or arbitrary JSON documents into the engine where sizes cannot be
 * predicted.
 * - **Fragmentation Resistance:** The Two-Level segregation ensures that we
 * don't suffer the massive internal fragmentation of a Buddy Allocator, making
 * it safe for long-running, high-churn memory pools.
 */
class TLSFAllocator
{
    storage::MemoryView mMemory;

    [[nodiscard]]
    details::TLSFHeapHeader *header() noexcept
    {
        return mMemory.cast_view<details::TLSFHeapHeader>();
    }

    details::TLSFBlockHeader *get_block(std::uint32_t offset);
    void mapping_insert(
        std::uint32_t size, std::uint32_t &fli, std::uint32_t &sli) const;
    void list_insert(std::uint32_t offset);
    void list_remove(std::uint32_t offset);

public:
    static constexpr std::uint8_t SIGNATURE =
        allocator_signature(AllocatorType::TLSF);

    TLSFAllocator(
        storage::MemoryView memory, std::uint32_t total_size,
        bool force_format = false);

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

static_assert(ReallocatableAllocator<TLSFAllocator>);
} // namespace usagi::runtime::allocators
