#pragma once

#include <Usagi/Platforms/Instructions/BitManipulations.hpp>
#include <Usagi/Runtime/Allocators/Concepts/FixedSizeAllocator.hpp>
#include <Usagi/Runtime/Errors/Errors.hpp>
#include <Usagi/Runtime/Storage/Views/MemoryView.hpp>

#include "AllocatorCommon.hpp"

namespace usagi::runtime::allocators
{
namespace details
{
/**
 * \brief The header mapped at the base of the hive allocator's memory region.
 */
struct HiveHeapHeader
{
    static constexpr std::uint64_t EXPECTED_MAGIC =
        0x5553'4147'4849'5645; // "USAGIHIVE"

    std::uint64_t magic;
    std::uint32_t data_payload_size;
    std::uint32_t payload_alignment;
    std::uint32_t blocks_per_page;
    std::uint32_t page_size;

    alignas(4) OperandBitWidth mask_width;

    std::uint32_t first_page;
    std::uint32_t last_page;
    std::uint32_t first_free_page;

    std::uint32_t max_pages;
    std::uint32_t allocated_pages;
    std::uint64_t page_seq_id_counter;

    [[nodiscard]]
    auto expected_magic() const noexcept
    {
        return EXPECTED_MAGIC ^ std::to_underlying(mask_width);
    }
};

static_assert(sizeof(HiveHeapHeader) == 56);

struct HivePageHeader
{
    static constexpr std::uint32_t INVALID_PAGE = -1;

    std::uint32_t next_page;
    std::uint32_t prev_page;
    std::uint32_t next_free_page;
    std::uint32_t prev_free_page;
    std::uint64_t page_seq_id;
    std::uint64_t free_mask;

    [[nodiscard]]
    bool is_full() const noexcept
    {
        return free_mask == 0;
    }
};

static_assert(sizeof(HivePageHeader) == 32);
} // namespace details

/**
 * Shio:
 * HiveAllocator manages fixed-size blocks by grouping them into discrete
 * "pages". Each page contains its own bitmap tracking local vacancies. This
 * provides excellent cache locality during iteration and eliminates the
 * overhead of scanning a massive global bitmap. When a page is full, it is
 * removed from the free list. When an element is freed, the page is re-linked
 * to the free list if it was previously full.
 */
class alignas(64) HiveAllocator
{
    mutable storage::MemoryView mMemory;
    OperandBitWidth             mBitsPerElement;

    [[nodiscard]]
    details::HiveHeapHeader *header() const
    {
        return mMemory.cast_view<details::HiveHeapHeader>(0);
    }

public:
    using PageHeader = details::HivePageHeader;

    static constexpr std::uint8_t SIGNATURE =
        allocator_signature(AllocatorType::Hive);

    HiveAllocator(storage::MemoryView memory, OperandBitWidth mask_width);

    void initialize(std::size_t block_size, std::size_t alignment);

    [[nodiscard]]
    MemoryHandle allocate();

    void deallocate(const MemoryHandle &handle);

    [[nodiscard]]
    std::uint64_t block_size() const;

    [[nodiscard]]
    storage::StorageAlignment alignment() const;

    [[nodiscard]]
    std::uint32_t first_populated_page() const;

    [[nodiscard]]
    void *resolve(const MemoryHandle &handle) const;

    [[nodiscard]]
    PageHeader *resolve_page(std::uint32_t page_idx) const;

    [[nodiscard]]
    void *resolve_page_data_base(std::uint32_t page_idx) const;
};

static_assert(FixedSizeAllocator<HiveAllocator>);
} // namespace usagi::runtime::allocators
