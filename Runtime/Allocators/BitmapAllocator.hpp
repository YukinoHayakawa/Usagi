#pragma once

#include <cstring>
#include <utility>

#include <Usagi/Platforms/Instructions/BitManipulations.hpp>
#include <Usagi/Runtime/Allocators/Concepts/FixedSizeAllocator.hpp>

#include "AllocatorCommon.hpp"

namespace usagi::runtime::allocators
{
/**
 * \brief The header mapped at the base of the bitmap allocator's memory region.
 */
template <OperandBitWidth Width>
struct BitmapHeapHeader
{
    static constexpr std::uint64_t EXPECTED_MAGIC =
        0x5553'4147'4950'4147; // "USAGIPAG"

    using MaskType = platforms::instructions::
        DefaultBitManipulationInstructions<Width>::ValueType;

    std::uint64_t magic;
    std::uint32_t block_size;
    std::uint32_t block_alignment;
    std::uint32_t max_blocks;

    // The index of the first block that can be allocated (skipping the header's
    // own blocks)
    std::uint32_t first_data_block;
    std::uint32_t active_blocks;

    // Flexible array member. 1 bit = 1 block. 1 = Free, 0 = Used.
    // todo: flexible array is a C99 feature
    MaskType free_mask[];
};

/**
 * \brief A Position-Independent, Bitset-based Fixed Size Allocator.
 *
 * Shio:
 * This allocator divides the MemoryView into fixed-size blocks and returns
 * `MemoryHandle`s. Because it uses dense bitmasks (`_tzcnt_u64`), allocation is
 * strictly O(1), highly deterministic, and completely lock-free if synchronized
 * externally.
 */
template <
    OperandBitWidth Width = OperandBitWidth::_64Bit,
    typename BitOps =
        platforms::instructions::DefaultBitManipulationInstructions<Width>
>
    requires platforms::instructions::BitManipulationInstructions<BitOps> &&
    (Width == OperandBitWidth::_32Bit || Width == OperandBitWidth::_64Bit)
class BitmapAllocator
{
    storage::MemoryView mMemory;

    using HeaderType = BitmapHeapHeader<Width>;
    using MaskType   = BitOps::ValueType;

    static constexpr std::uint32_t BITS_PER_ELEMENT = to_bits(Width);

    [[nodiscard]]
    HeaderType *header() noexcept
    {
        return mMemory.cast_view<HeaderType>();
    }

public:
    static constexpr std::uint8_t SIGNATURE = allocator_signature(
        AllocatorType::Bitmap, Width == OperandBitWidth::_64Bit);

    BitmapAllocator(
        storage::MemoryView memory, std::uint32_t block_size,
        const storage::StorageAlignment alignment, std::uint32_t max_blocks,
        bool force_format = false)
        : mMemory(std::move(memory))
    {
        const std::uint64_t alignment_bytes = to_bytes(alignment);

        USAGI_CHECK_THROW(
            LogicException,
            block_size > 0 && block_size % alignment_bytes == 0,
            "block_size must be a multiple of alignment");

        const std::uint32_t mask_elements =
            (max_blocks + (BITS_PER_ELEMENT - 1)) / BITS_PER_ELEMENT;
        const std::size_t header_bytes =
            sizeof(HeaderType) + (mask_elements * sizeof(MaskType));

        const std::size_t header_blocks =
            (header_bytes + block_size - 1) / block_size;
        const std::size_t total_bytes =
            static_cast<std::size_t>(max_blocks) * block_size;

        USAGI_CHECK_THROW(
            LogicException,
            mMemory.max_size() >= total_bytes,
            "MemoryView is too small for requested capacity.");

        mMemory.commit(
            0, header_blocks * block_size, storage::CommitStrategy::Bypass);

        if(force_format || header()->magic != HeaderType::EXPECTED_MAGIC)
        {
            header()->magic            = HeaderType::EXPECTED_MAGIC;
            header()->block_size       = block_size;
            header()->block_alignment  = std::to_underlying(alignment);
            header()->max_blocks       = max_blocks;
            header()->first_data_block = header_blocks;
            header()->active_blocks    = 0;

            std::memset(
                header()->free_mask, 0xFF, mask_elements * sizeof(MaskType));

            for(std::uint32_t i = 0; i < header_blocks; ++i)
            {
                const std::uint32_t elem_idx = i / BITS_PER_ELEMENT;
                const std::uint32_t bit_idx  = i % BITS_PER_ELEMENT;
                header()->free_mask[elem_idx] &=
                    ~(static_cast<MaskType>(1) << bit_idx);
            }

            if(const std::uint32_t excess_bits =
                    (mask_elements * BITS_PER_ELEMENT) - max_blocks;
                excess_bits > 0)
            {
                const MaskType valid_mask =
                    (static_cast<MaskType>(1)
                        << (BITS_PER_ELEMENT - excess_bits)) -
                    1;
                header()->free_mask[mask_elements - 1] &= valid_mask;
            }
        }
        else
        {
            USAGI_CHECK_THROW(
                BrokenInvariantException,
                header()->block_size == block_size,
                "Block size mismatch in persistent/mapped memory");
            USAGI_CHECK_THROW(
                BrokenInvariantException,
                header()->block_alignment == std::to_underlying(alignment),
                "Block alignment mismatch in persistent/mapped memory");
            USAGI_CHECK_THROW(
                BrokenInvariantException,
                header()->max_blocks <= max_blocks,
                "Mapped memory has more blocks than requested");
        }
    }

    [[nodiscard]]
    MemoryHandle allocate()
    {
        const std::uint32_t mask_elements =
            (header()->max_blocks + (BITS_PER_ELEMENT - 1)) / BITS_PER_ELEMENT;

        for(std::uint32_t i = 0; i < mask_elements; ++i)
        {
            if(const MaskType mask = header()->free_mask[i]; mask != 0)
            {
                const std::uint32_t bit_idx = static_cast<std::uint32_t>(
                    BitOps::count_trailing_zeros(mask));

                header()->free_mask[i] = BitOps::reset_lowest_set_bit(mask);
                ++header()->active_blocks;

                const std::uint32_t block_id = (i * BITS_PER_ELEMENT) + bit_idx;
                const std::uint64_t offset =
                    static_cast<std::uint64_t>(block_id) * header()->block_size;

                return { SIGNATURE, offset, header()->block_size };
            }
        }

        USAGI_CHECK_THROW(
            ResourceExhaustedException,
            false,
            "Out of memory blocks in BitmapAllocator");
        errors::unreachable();
    }

    void deallocate(MemoryHandle handle)
    {
        if(!handle.is_valid()) return;

        USAGI_CHECK_THROW(
            LogicException,
            handle.signature == SIGNATURE,
            "Invalid MemoryHandle signature for BitmapAllocator");

        const std::uint32_t block_id =
            static_cast<std::uint32_t>(handle.offset / header()->block_size);

        USAGI_CHECK_THROW(
            LogicException,
            block_id >= header()->first_data_block &&
                block_id < header()->max_blocks,
            "Invalid block ID (Handle offset out of bounds)");

        const std::uint32_t i       = block_id / BITS_PER_ELEMENT;
        const std::uint32_t bit_idx = block_id % BITS_PER_ELEMENT;

        header()->free_mask[i] |= (static_cast<MaskType>(1) << bit_idx);
        --header()->active_blocks;
    }

    [[nodiscard]]
    void *resolve(const MemoryHandle handle) noexcept
    {
        validate_before_resolve(this, handle);
        return handle.resolve(mMemory);
    }

    [[nodiscard]]
    std::uint64_t block_size() const noexcept
    {
        return header()->block_size;
    }

    [[nodiscard]]
    storage::StorageAlignment alignment() const noexcept
    {
        return static_cast<storage::StorageAlignment>(
            header()->block_alignment);
    }

    [[nodiscard]]
    const storage::MemoryView &view() const noexcept
    {
        return mMemory;
    }
};

// Shio: FixedSizeAllocator concept check
static_assert(FixedSizeAllocator<BitmapAllocator<OperandBitWidth::_64Bit>>);
static_assert(FixedSizeAllocator<BitmapAllocator<OperandBitWidth::_32Bit>>);
} // namespace usagi::runtime::allocators
