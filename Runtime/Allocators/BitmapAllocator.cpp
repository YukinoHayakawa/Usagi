#include "BitmapAllocator.hpp"

#include <Usagi/Library/Utilities/Bits.hpp>

namespace usagi::runtime::allocators
{
using namespace details;

BitmapAllocatorBase::BitmapAllocatorBase(
    storage::MemoryView memory, const std::uint32_t bits_per_element) noexcept
    : mMemory(std::move(memory)), mBitsPerElement(bits_per_element)
{
}

void BitmapAllocatorBase::initialize(
    const std::uint32_t block_size, const storage::StorageAlignment alignment,
    const std::uint32_t max_blocks, const bool force_format)
{
    const std::uint64_t alignment_bytes = to_bytes(alignment);

    USAGI_CHECK_THROW(
        LogicException,
        block_size > 0 && block_size % alignment_bytes == 0,
        "block_size must be a multiple of alignment");

    BitmapHeapHeaderBase dummy_header;

    dummy_header.bit_width  = mBitsPerElement;
    dummy_header.max_blocks = max_blocks;

    const std::uint32_t mask_elements = dummy_header.num_mask_elements();
    const std::size_t   header_bytes  = dummy_header.num_header_bytes();

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

    auto *h = mMemory.cast_view<BitmapHeapHeaderBase>();

    const std::uint64_t expected_magic_val =
        BitmapHeapHeaderBase::EXPECTED_MAGIC ^ mBitsPerElement;

    if(force_format || h->magic != expected_magic_val)
    {
        h->magic            = expected_magic_val;
        h->bit_width        = mBitsPerElement;
        h->block_size       = block_size;
        h->block_alignment  = std::to_underlying(alignment);
        h->max_blocks       = max_blocks;
        h->first_data_block = static_cast<std::uint32_t>(header_blocks);
        h->active_blocks    = 0;

        h->act([](auto *h_typed) {
            std::memset(
                h_typed->free_masks, 0xFF, h_typed->num_free_mask_bytes());
        });

        for(std::uint32_t i = 0; i < header_blocks; ++i)
        {
            const std::uint32_t elem_idx = i / mBitsPerElement;
            const std::uint32_t bit_idx  = i % mBitsPerElement;
            // todo: profile whether it's better to enclose the whole for loop
            h->act([=](auto *h_typed) {
                bits::clear_bit(h_typed->free_masks[elem_idx], bit_idx);
            });
        }

        if(const std::uint32_t excess_bits =
                (mask_elements * mBitsPerElement) - max_blocks;
            excess_bits > 0)
        {
            // todo: profile whether it's better to enclose the whole for loop
            h->act([&](auto *h_typed) {
                using MaskType =
                    std::remove_reference_t<decltype(h_typed->free_masks[0])>;
                const auto valid_mask = bits::create_mask_with_excess<MaskType>(
                    h->bit_width, excess_bits);
                bits::bit_and(
                    h_typed->free_masks[mask_elements - 1], valid_mask);
            });
        }
    }
    else
    {
        USAGI_CHECK_THROW(
            BrokenInvariantException,
            h->bit_width == mBitsPerElement,
            "Bit width mismatch in persistent/mapped memory");
        USAGI_CHECK_THROW(
            BrokenInvariantException,
            h->block_size == block_size,
            "Block size mismatch in persistent/mapped memory");
        USAGI_CHECK_THROW(
            BrokenInvariantException,
            h->block_alignment == std::to_underlying(alignment),
            "Block alignment mismatch in persistent/mapped memory");
        USAGI_CHECK_THROW(
            BrokenInvariantException,
            h->max_blocks <= max_blocks,
            "Mapped memory has more blocks than requested");
    }
}

MemoryHandle BitmapAllocatorBase::allocate_impl(const std::uint8_t signature)
{
    auto *h = mMemory.cast_view<BitmapHeapHeaderBase>();

    const std::uint32_t mask_elements = h->num_mask_elements();
    const std::uint32_t block_id = h->act([&](auto *h_typed) -> std::uint32_t {
        for(std::uint32_t i = 0; i < mask_elements; ++i)
        {
            if(h_typed->free_masks[i] != 0)
            {
                const std::uint32_t bit_idx =
                    bits::pop_lowest_set_bit(h_typed->free_masks[i]);
                return (i * mBitsPerElement) + bit_idx;
            }
        }
        return static_cast<std::uint32_t>(-1);
    });

    USAGI_CHECK_THROW(
        OutOfMemoryException,
        block_id != static_cast<std::uint32_t>(-1),
        "Out of memory blocks in BitmapAllocator");

    ++h->active_blocks;

    const std::uint64_t offset =
        static_cast<std::uint64_t>(block_id) * h->block_size;

    return {
        .signature = signature,
        .offset    = offset,
        .size      = h->block_size,
    };
}

void BitmapAllocatorBase::deallocate_impl(
    const MemoryHandle handle, const std::uint8_t signature)
{
    if(!handle.is_valid()) return;

    USAGI_CHECK_THROW(
        LogicException,
        handle.signature == signature,
        "Invalid MemoryHandle signature for BitmapAllocator");

    auto *h = mMemory.cast_view<BitmapHeapHeaderBase>();

    const std::uint32_t block_id =
        h->block_id_from_offset(static_cast<std::uint32_t>(handle.offset));

    USAGI_CHECK_THROW(
        LogicException,
        block_id >= h->first_data_block && block_id < h->max_blocks,
        "Invalid block ID (Handle offset out of bounds)");

    const std::uint32_t i       = block_id / mBitsPerElement;
    const std::uint32_t bit_idx = block_id % mBitsPerElement;

    h->act(
        [&](auto *h_typed) { bits::set_bit(h_typed->free_masks[i], bit_idx); });

    --h->active_blocks;
}
} // namespace usagi::runtime::allocators
