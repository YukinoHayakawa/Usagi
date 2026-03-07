#include "BitmapAllocator.hpp"

#include <utility>

#include <Usagi/Runtime/Errors/Exceptions.hpp>

// Include BMI1 intrinsics
#ifdef _MSC_VER
    #include <intrin.h>
#else
    #include <x86intrin.h>
#endif

namespace usagi::runtime::allocators
{
BitmapAllocator::BitmapAllocator(MemoryView memory,
    const std::uint32_t                     block_size,
    const storage::StorageAlignment         alignment,
    const std::uint32_t                     max_blocks,
    const bool                              force_format)
    : mMemory(std::move(memory))
{
    const std::uint64_t alignment_bytes = storage::to_bytes(alignment);
    USAGI_ASSERT_THROW(block_size > 0 && block_size % alignment_bytes == 0,
        std::invalid_argument("block_size must be a multiple of alignment"));

    // Calculate the exact size needed for the bitmask
    const std::uint32_t mask_elements = (max_blocks + 63) / 64;
    const std::size_t   header_bytes =
        sizeof(BitmapHeapHeader) + (mask_elements * sizeof(std::uint64_t));

    // The header itself consumes some number of blocks at the start of the
    // memory
    const std::uint32_t header_blocks = static_cast<std::uint32_t>(
        (header_bytes + block_size - 1) / block_size);
    const std::size_t total_bytes =
        static_cast<std::size_t>(max_blocks) * block_size;

    // We must ensure the view has enough space reserved
    USAGI_ASSERT_THROW(mMemory.max_size() >= total_bytes,
        std::invalid_argument(
            "MemoryView is too small for requested capacity."));

    // Commit enough pages for the header immediately
    mMemory.commit(0, header_blocks * block_size);

    // If formatting is forced (e.g. fresh volatile RAM) or the magic number
    // doesn't match (e.g. newly created memory-mapped file), we initialize the
    // header.
    if(force_format || header()->magic != BitmapHeapHeader::EXPECTED_MAGIC)
    {
        header()->magic            = BitmapHeapHeader::EXPECTED_MAGIC;
        header()->block_size       = block_size;
        header()->block_alignment  = std::to_underlying(alignment);
        header()->max_blocks       = max_blocks;
        header()->first_data_block = header_blocks;
        header()->active_blocks    = 0;

        // Initialize all bits to 1 (Free)
        std::memset(
            header()->free_mask, 0xFF, mask_elements * sizeof(std::uint64_t));

        // Mark the blocks consumed by the header itself as Used (0)
        for(std::uint32_t i = 0; i < header_blocks; ++i)
        {
            const std::uint32_t elem_idx = i / 64;
            const std::uint32_t bit_idx  = i % 64;
            header()->free_mask[elem_idx] &= ~(1ULL << bit_idx);
        }

        // Mask out the excess bits in the very last uint64_t element to
        // prevent out-of-bounds allocation
        if(const std::uint32_t excess_bits = (mask_elements * 64) - max_blocks;
            excess_bits > 0)
        {
            const std::uint64_t valid_mask = (1ULL << (64 - excess_bits)) - 1;
            header()->free_mask[mask_elements - 1] &= valid_mask;
        }
    }
    else
    {
        // We loaded an existing valid header. Verify it matches our code's
        // expectations.
        USAGI_ASSERT_THROW(header()->block_size == block_size,
            std::runtime_error(
                "Block size mismatch in persistent/mapped memory"));
        USAGI_ASSERT_THROW(
            header()->block_alignment == std::to_underlying(alignment),
            std::runtime_error(
                "Block alignment mismatch in persistent/mapped memory"));
        USAGI_ASSERT_THROW(header()->max_blocks <= max_blocks,
            std::runtime_error("Mapped memory has more blocks than requested"));
    }
}

MemoryHandle BitmapAllocator::allocate()
{
    const std::uint32_t mask_elements = (header()->max_blocks + 63) / 64;

    // Fast SIMD scan for the first free block
    for(std::uint32_t i = 0; i < mask_elements; ++i)
    {
        if(const std::uint64_t mask = header()->free_mask[i]; mask != 0)
        {
            // Find index of lowest set bit (first free block) using BMI1
            // instruction
            const std::uint32_t bit_idx =
                static_cast<std::uint32_t>(_tzcnt_u64(mask));

            // Clear the bit (mark as used) using BMI1 blsr instruction.
            // This is mathematically equivalent to `mask & (mask - 1)` but
            // executes in 1 cycle.
            header()->free_mask[i] = _blsr_u64(mask);
            header()->active_blocks++;

            const std::uint32_t block_id = (i * 64) + bit_idx;
            const std::uint64_t offset =
                static_cast<std::uint64_t>(block_id) * header()->block_size;

            return { SIGNATURE, offset };
        }
    }

    USAGI_ASSERT_THROW(false, std::bad_alloc()); // Out of memory blocks
}

void BitmapAllocator::deallocate(const MemoryHandle handle)
{
    if(!handle.is_valid()) return;

    USAGI_ASSERT_THROW(handle.signature == SIGNATURE,
        std::invalid_argument(
            "Invalid MemoryHandle signature for BitmapAllocator"));

    const std::uint32_t block_id =
        static_cast<std::uint32_t>(handle.offset / header()->block_size);

    USAGI_ASSERT_THROW(block_id >= header()->first_data_block &&
            block_id < header()->max_blocks,
        std::out_of_range("Invalid block ID (Handle offset out of bounds)"));

    const std::uint32_t i       = block_id / 64;
    const std::uint32_t bit_idx = block_id % 64;

    // Mark the bit as 1 (Free)
    header()->free_mask[i] |= (1ULL << bit_idx);
    header()->active_blocks--;
}

void *BitmapAllocator::resolve(const MemoryHandle handle) const noexcept
{
    assert(!handle.is_valid() || handle.signature == SIGNATURE);
    return handle.resolve(mMemory);
}

std::uint64_t BitmapAllocator::block_size() const noexcept
{
    return header()->block_size;
}

storage::StorageAlignment BitmapAllocator::alignment() const noexcept
{
    return static_cast<storage::StorageAlignment>(header()->block_alignment);
}
} // namespace usagi::runtime::allocators
