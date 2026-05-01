#include "HiveAllocator.hpp"

#include <Usagi/Library/Utilities/Bits.hpp>
#include <Usagi/Platforms/Syscalls/VirtualMemory.hpp>

namespace usagi::runtime::allocators
{
using namespace details;

namespace
{
using BitOps = platforms::instructions::
    DefaultBitManipulationInstructions<OperandBitWidth::_64Bit>;
} // namespace

HiveAllocator::HiveAllocator(
    storage::MemoryView memory, const OperandBitWidth mask_width)
    : mMemory(std::move(memory)), mBitsPerElement(mask_width)
{
}

void HiveAllocator::initialize(
    const std::size_t block_size, const std::size_t alignment)
{
    const std::size_t total_bytes = mMemory.max_size();

    // Ensure block size meets alignment
    const std::size_t aligned_block_size =
        BitOps::align_up_pow2(block_size, alignment);

    const std::size_t max_blocks_per_page = to_bits(mBitsPerElement);

    // Page header + data blocks
    const std::size_t min_page_size =
        BitOps::align_up_pow2(sizeof(HivePageHeader), alignment) +
        (aligned_block_size * max_blocks_per_page);

    // Align page size to system granularity
    const std::size_t page_size = BitOps::align_up_pow2(
        min_page_size, platforms::memory::allocation_granularity());

    USAGI_CHECK_THROW(
        LogicException,
        total_bytes >= page_size * 2,
        "MemoryView is too small to host a HiveAllocator.");

    // Commit first page for the heap header (which acts as page 0)
    mMemory.commit(0, page_size, storage::CommitStrategy::Bypass);

    auto *h = header();

    h->mask_width        = mBitsPerElement;
    h->magic             = h->expected_magic();
    h->data_payload_size = static_cast<std::uint32_t>(aligned_block_size);
    h->payload_alignment = static_cast<std::uint32_t>(alignment);
    h->blocks_per_page   = static_cast<std::uint32_t>(max_blocks_per_page);
    h->page_size         = static_cast<std::uint32_t>(page_size);

    h->first_page      = HivePageHeader::INVALID_PAGE;
    h->last_page       = HivePageHeader::INVALID_PAGE;
    h->first_free_page = HivePageHeader::INVALID_PAGE;

    h->max_pages       = static_cast<std::uint32_t>(total_bytes / page_size);
    h->allocated_pages = 1; // Page 0 is reserved for the HeapHeader
    h->page_seq_id_counter = 0;
}

MemoryHandle HiveAllocator::allocate()
{
    auto *h = header();

    USAGI_CHECK_THROW(
        BrokenInvariantException,
        h->magic == h->expected_magic(),
        "Invalid HiveHeapHeader magic.");

    auto get_page = [this, h](const std::uint32_t idx) -> HivePageHeader * {
        if(idx == HivePageHeader::INVALID_PAGE) return nullptr;
        return mMemory.cast_view<HivePageHeader>(
            static_cast<std::uint64_t>(idx) * h->page_size);
    };

    auto link_free_page = [&](const std::uint32_t page_idx) {
        auto *page           = get_page(page_idx);
        page->prev_free_page = HivePageHeader::INVALID_PAGE;
        page->next_free_page = h->first_free_page;
        if(h->first_free_page != HivePageHeader::INVALID_PAGE)
        {
            get_page(h->first_free_page)->prev_free_page = page_idx;
        }
        h->first_free_page = page_idx;
    };

    auto unlink_free_page = [&](const std::uint32_t page_idx) {
        auto *page = get_page(page_idx);
        if(page->prev_free_page != HivePageHeader::INVALID_PAGE)
        {
            get_page(page->prev_free_page)->next_free_page =
                page->next_free_page;
        }
        else
        {
            h->first_free_page = page->next_free_page;
        }

        if(page->next_free_page != HivePageHeader::INVALID_PAGE)
        {
            get_page(page->next_free_page)->prev_free_page =
                page->prev_free_page;
        }
        page->prev_free_page = HivePageHeader::INVALID_PAGE;
        page->next_free_page = HivePageHeader::INVALID_PAGE;
    };

    const bool is_64_bit = h->mask_width == OperandBitWidth::_64Bit;

    if(h->first_free_page == HivePageHeader::INVALID_PAGE)
    {
        USAGI_CHECK_THROW(
            OutOfMemoryException,
            h->allocated_pages < h->max_pages,
            "HiveAllocator out of pages");

        const std::uint32_t new_page_idx = h->allocated_pages++;
        const std::uint64_t offset =
            static_cast<std::uint64_t>(new_page_idx) * h->page_size;

        mMemory.commit(offset, h->page_size, storage::CommitStrategy::Bypass);

        auto *page           = get_page(new_page_idx);
        page->next_page      = HivePageHeader::INVALID_PAGE;
        page->prev_page      = h->last_page;
        page->next_free_page = HivePageHeader::INVALID_PAGE;
        page->prev_free_page = HivePageHeader::INVALID_PAGE;
        page->page_seq_id    = h->page_seq_id_counter++;

        if(is_64_bit) [[likely]]
        {
            page->free_mask =
                bits::create_mask<std::uint64_t>(h->blocks_per_page);
        }
        else
        {
            page->free_mask =
                bits::create_mask<std::uint32_t>(h->blocks_per_page);
        }

        if(h->last_page != HivePageHeader::INVALID_PAGE)
        {
            get_page(h->last_page)->next_page = new_page_idx;
        }
        else
        {
            h->first_page = new_page_idx;
        }
        h->last_page = new_page_idx;

        link_free_page(new_page_idx);
    }

    const std::uint32_t page_idx = h->first_free_page;
    auto               *page     = get_page(page_idx);

    std::uint32_t local_idx;
    if(is_64_bit) [[likely]]
    {
        local_idx = bits::pop_lowest_set_bit(page->free_mask);
    }
    else
    {
        // Must downcast explicitly so TZCNT processes a 32-bit integer cleanly
        auto mask_32    = static_cast<std::uint32_t>(page->free_mask);
        local_idx       = bits::pop_lowest_set_bit(mask_32);
        page->free_mask = mask_32;
    }

    if(page->is_full())
    {
        unlink_free_page(page_idx);
    }

    const std::uint64_t logical_offset =
        (static_cast<std::uint64_t>(page_idx) << 32) | local_idx;

    return MemoryHandle {
        .signature = allocator_signature(AllocatorType::Hive, is_64_bit),
        .offset    = logical_offset,
        .size      = h->data_payload_size,
    };
}

void HiveAllocator::deallocate(const MemoryHandle &handle)
{
    if(!handle.is_valid()) return;

    auto *h = header();

    const bool is_64_bit = h->mask_width == OperandBitWidth::_64Bit;

    USAGI_CHECK_FATAL(
        handle.signature == allocator_signature(AllocatorType::Hive, is_64_bit),
        "Invalid MemoryHandle signature in HiveAllocator");

    const std::uint32_t page_idx =
        static_cast<std::uint32_t>(handle.offset >> 32);
    const std::uint32_t local_idx =
        static_cast<std::uint32_t>(handle.offset & 0xFFFF'FFFFu);

    auto *page = mMemory.cast_view<HivePageHeader>(
        static_cast<std::uint64_t>(page_idx) * h->page_size);

    const bool was_full = page->is_full();

    if(is_64_bit) [[likely]]
    {
        bits::set_bit(page->free_mask, local_idx);
    }
    else
    {
        auto mask_32 = static_cast<std::uint32_t>(page->free_mask);
        bits::set_bit(mask_32, local_idx);
        page->free_mask = mask_32;
    }

    if(was_full)
    {
        // Link free page inline logic
        page->prev_free_page = HivePageHeader::INVALID_PAGE;
        page->next_free_page = h->first_free_page;
        if(h->first_free_page != HivePageHeader::INVALID_PAGE)
        {
            auto *first_free = mMemory.cast_view<HivePageHeader>(
                static_cast<std::uint64_t>(h->first_free_page) * h->page_size);
            first_free->prev_free_page = page_idx;
        }
        h->first_free_page = page_idx;
    }
}

std::uint64_t HiveAllocator::block_size() const
{
    return header()->data_payload_size;
}

storage::StorageAlignment HiveAllocator::alignment() const
{
    return static_cast<storage::StorageAlignment>(header()->payload_alignment);
}

std::uint32_t HiveAllocator::first_populated_page() const
{
    return header()->first_page;
}

void *HiveAllocator::resolve(const MemoryHandle &handle) const
{
    // Shio: This function is deprecated for AOSOA usage since HiveAllocator
    // now provides logical indices instead of direct physical offsets, but
    // we implement it for standard FixedSizeAllocator compliance.
    if(!handle.is_valid()) return nullptr;

    const std::uint32_t page_idx =
        static_cast<std::uint32_t>(handle.offset >> 32);
    const std::uint32_t local_idx =
        static_cast<std::uint32_t>(handle.offset & 0xFFFF'FFFFu);

    const auto *h = header();
    return static_cast<std::byte *>(resolve_page_data_base(page_idx)) +
        (local_idx * h->data_payload_size);
}

HiveAllocator::PageHeader *HiveAllocator::resolve_page(
    const std::uint32_t page_idx) const
{
    if(page_idx == HivePageHeader::INVALID_PAGE) return nullptr;
    const auto offset =
        static_cast<std::uint64_t>(page_idx) * header()->page_size;
    return mMemory.cast_view<PageHeader>(offset);
}

void *HiveAllocator::resolve_page_data_base(const std::uint32_t page_idx) const
{
    const auto         *h = header();
    const std::uint64_t data_start_offset =
        (static_cast<std::uint64_t>(page_idx) * h->page_size) +
        BitOps::align_up_pow2(sizeof(PageHeader), h->payload_alignment);

    return mMemory.cast_view<void>(data_start_offset);
}
} // namespace usagi::runtime::allocators
