#pragma once

#include <cstring>
#include <utility>

#include <Usagi/Library/Values/ValueList.hpp>
#include <Usagi/Platforms/Instructions/BitManipulations.hpp>
#include <Usagi/Runtime/Allocators/Concepts/FixedSizeAllocator.hpp>

#include "AllocatorCommon.hpp"

namespace usagi::runtime::allocators
{
namespace details
{
template <OperandBitWidth Width>
struct BitmapHeapHeader;

/*
 * We have to make sure this struct doesn't contain pointers while making it
 * polymorphic!
 */
struct BitmapHeapHeaderBase
{
    static constexpr std::uint64_t EXPECTED_MAGIC =
        0x5553'4147'4950'4147; // "USAGIPAG"

    std::uint64_t magic           = EXPECTED_MAGIC;
    std::uint32_t bit_width       = -1; // 32 or 64
    std::uint32_t block_size      = 0;
    std::uint32_t block_alignment = 0;
    std::uint32_t max_blocks      = -1;

    // The index of the first block that can be allocated (skipping the header's
    // own blocks)
    std::uint32_t first_data_block = -1;
    std::uint32_t active_blocks    = -1;

    [[nodiscard]]
    std::uint64_t expected_magic() const noexcept
    {
        // make the magic different for _32/_64
        return EXPECTED_MAGIC ^ bit_width;
    }

    [[nodiscard]]
    std::uint32_t num_mask_elements() const
    {
        return (max_blocks + (bit_width - 1)) / bit_width;
    }

    [[nodiscard]]
    std::size_t num_header_bytes() const noexcept
    {
        return sizeof(BitmapHeapHeaderBase) +
            (num_mask_elements() * bit_width >> bit_rshift_bits_to_byte);
    }

    [[nodiscard]]
    std::size_t num_free_mask_bytes() const noexcept
    {
        return num_mask_elements() * bit_width >> bit_rshift_bits_to_byte;
    }

    [[nodiscard]]
    std::uint32_t block_id_from_offset(
        const std::uint32_t offset) const noexcept
    {
        return offset / block_size;
    }

    template <OperandBitWidth Width>
    BitmapHeapHeader<Width> *width_dependent();

    decltype(auto) act(auto &&op)
    {
        if(bit_width == 32)
        {
            return op(width_dependent<OperandBitWidth::_32Bit>());
        }
        if(bit_width == 64)
        {
            return op(width_dependent<OperandBitWidth::_64Bit>());
        }
        errors::unreachable("invalid operand bit width");
    }
};

/**
 * \brief The header mapped at the base of the bitmap allocator's memory region.
 */
template <OperandBitWidth Width>
struct BitmapHeapHeader : BitmapHeapHeaderBase
{
    using MaskType = platforms::instructions::
        DefaultBitManipulationInstructions<Width>::ValueType;

    // Flexible array member. 1 bit = 1 block. 1 = Free, 0 = Used.
    // todo: this is a C99 extension
    MaskType free_masks[];
};

template <OperandBitWidth Width>
BitmapHeapHeader<Width> *BitmapHeapHeaderBase::width_dependent()
{
    return static_cast<BitmapHeapHeader<Width> *>(this);
}
} // namespace details

/**
 * \brief Base logic for BitmapAllocator to reduce compile times.
 */
class BitmapAllocatorBase
{
protected:
    mutable storage::MemoryView mMemory;
    std::uint32_t               mBitsPerElement;

    [[nodiscard]]
    details::BitmapHeapHeaderBase *header() const noexcept
    {
        return mMemory.cast_view<details::BitmapHeapHeaderBase>();
    }

    BitmapAllocatorBase(
        storage::MemoryView memory, std::uint32_t bits_per_element) noexcept;

    void initialize(
        std::uint32_t block_size, storage::StorageAlignment alignment,
        std::uint32_t max_blocks, bool force_format);

    [[nodiscard]]
    MemoryHandle allocate_impl(std::uint8_t signature);

    void deallocate_impl(MemoryHandle handle, std::uint8_t signature);

public:
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

/**
 * \brief A Position-Independent, Bitset-based Fixed Size Allocator.
 *
 * Shio:
 * This allocator divides the MemoryView into fixed-size blocks and returns
 * `MemoryHandle`s. Because it uses dense bitmasks (`_tzcnt_u64`), allocation is
 * strictly O(1), highly deterministic, and completely lock-free if synchronized
 * externally.
 */
template <OperandBitWidth Width = OperandBitWidth::_64Bit>
    requires AnyOf<Width, OperandBitWidth::_32Bit, OperandBitWidth::_64Bit>
class BitmapAllocator : public BitmapAllocatorBase
{
public:
    static constexpr std::uint8_t SIGNATURE = allocator_signature(
        AllocatorType::Bitmap, Width == OperandBitWidth::_64Bit);

    BitmapAllocator(
        storage::MemoryView memory, const std::uint32_t block_size,
        const storage::StorageAlignment alignment,
        const std::uint32_t max_blocks, const bool force_format = false)
        : BitmapAllocatorBase(std::move(memory), to_bits(Width))
    {
        initialize(block_size, alignment, max_blocks, force_format);
    }

    [[nodiscard]]
    MemoryHandle allocate()
    {
        return allocate_impl(SIGNATURE);
    }

    void deallocate(const MemoryHandle handle)
    {
        deallocate_impl(handle, SIGNATURE);
    }

    [[nodiscard]]
    void *resolve(const MemoryHandle handle) noexcept
    {
        validate_before_resolve(this, handle);
        return handle.resolve(mMemory);
    }
};

// Shio: FixedSizeAllocator concept check
static_assert(FixedSizeAllocator<BitmapAllocator<OperandBitWidth::_64Bit>>);
static_assert(FixedSizeAllocator<BitmapAllocator<OperandBitWidth::_32Bit>>);
} // namespace usagi::runtime::allocators
