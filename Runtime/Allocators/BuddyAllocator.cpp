#include "BuddyAllocator.hpp"

#include <algorithm>

#include <Usagi/Runtime/Errors/Errors.hpp>

namespace usagi::runtime::allocators
{
BuddyBlockHeader *BuddyAllocator::get_block(const std::uint32_t offset)
{
    if(offset == 0) return nullptr;
    return mMemory.cast_view<BuddyBlockHeader>(offset);
}

void BuddyAllocator::list_remove(const std::uint32_t offset)
{
    BuddyBlockHeader   *block     = get_block(offset);
    const std::uint32_t order_idx = block->size_order - header()->min_order;

    if(block->prev_free != 0)
    {
        get_block(block->prev_free)->next_free = block->next_free;
    }
    else
    {
        header()->free_lists[order_idx] = block->next_free;
    }

    if(block->next_free != 0)
    {
        get_block(block->next_free)->prev_free = block->prev_free;
    }

    block->next_free = 0;
    block->prev_free = 0;
}

void BuddyAllocator::list_insert(const std::uint32_t offset)
{
    BuddyBlockHeader   *block     = get_block(offset);
    const std::uint32_t order_idx = block->size_order - header()->min_order;

    block->prev_free = 0;
    block->next_free = header()->free_lists[order_idx];

    if(header()->free_lists[order_idx] != 0)
    {
        get_block(header()->free_lists[order_idx])->prev_free = offset;
    }

    header()->free_lists[order_idx] = offset;
}

BuddyAllocator::BuddyAllocator(
    storage::MemoryView memory, const std::uint32_t min_alloc_size,
    const std::uint32_t total_size, const bool force_format)
    : mMemory(std::move(memory))
{
    USAGI_CHECK_THROW(
        LogicException,
        std::has_single_bit(min_alloc_size),
        "min_alloc_size must be a power of 2");
    USAGI_CHECK_THROW(
        LogicException,
        std::has_single_bit(total_size),
        "total_size must be a power of 2");
    USAGI_CHECK_THROW(
        LogicException,
        min_alloc_size >= sizeof(BuddyBlockHeader),
        "min_alloc_size must accommodate block header");
    USAGI_CHECK_THROW(
        LogicException,
        mMemory.max_size() >= total_size,
        "MemoryView is too small for requested capacity.");

    // Commit only enough space for the heap header to start
    mMemory.commit(0, sizeof(BuddyHeapHeader), storage::CommitStrategy::Exact);

    if(force_format || header()->magic != BuddyHeapHeader::EXPECTED_MAGIC)
    {
        header()->magic      = BuddyHeapHeader::EXPECTED_MAGIC;
        header()->min_order  = std::countr_zero(min_alloc_size);
        header()->max_order  = std::countr_zero(total_size);
        header()->total_size = total_size;

        constexpr std::uint32_t header_size = sizeof(BuddyHeapHeader);
        header()->payload_base_offset       = std::bit_ceil(header_size);

        for(int i = 0; i < 32; ++i)
        {
            header()->free_lists[i] = 0;
        }

        std::uint32_t current_offset = header()->payload_base_offset;
        std::uint32_t remaining_size = total_size - current_offset;

        while(remaining_size >= min_alloc_size)
        {
            std::uint32_t max_align_order = std::countr_zero(current_offset);
            std::uint32_t max_size_order =
                31 - std::countl_zero(remaining_size);

            std::uint32_t order = std::min(max_align_order, max_size_order);
            order               = std::max(order, header()->min_order);

            const std::uint32_t block_size = 1 << order;

            BuddyBlockHeader *block = get_block(current_offset);
            block->size_order       = order;
            block->is_free          = 1;

            list_insert(current_offset);

            current_offset += block_size;
            remaining_size -= block_size;
        }
    }
}

MemoryHandle BuddyAllocator::allocate(
    const std::uint64_t size, const storage::StorageAlignment alignment)
{
    std::uint64_t alignment_bytes = storage::to_bytes(alignment);
    // Ensure we have enough alignment for our uint32_t back-pointer
    alignment_bytes               = std::max<uint64_t>(alignment_bytes, 4);

    const std::uint32_t requested_size = static_cast<std::uint32_t>(size);
    const std::uint32_t max_padding =
        static_cast<std::uint32_t>(alignment_bytes - 1);
    const std::uint32_t required_size = requested_size +
        sizeof(BuddyBlockHeader) +
        sizeof(std::uint32_t) +
        max_padding;

    const std::uint32_t order = std::max(
        header()->min_order,
        static_cast<std::uint32_t>(std::bit_width(required_size - 1)));

    if(order > header()->max_order)
        USAGI_CHECK_THROW(
            OutOfMemoryException, false, "Requested size exceeds max_order");

    std::uint32_t current_order = order;
    while(current_order <= header()->max_order &&
        header()->free_lists[current_order - header()->min_order] == 0)
    {
        current_order++;
    }

    if(current_order > header()->max_order)
        USAGI_CHECK_THROW(
            OutOfMemoryException,
            false,
            "Out of memory blocks in BuddyAllocator"); // OOM

    const std::uint32_t offset =
        header()->free_lists[current_order - header()->min_order];
    list_remove(offset);

    while(current_order > order)
    {
        current_order--;
        const std::uint32_t buddy_offset = offset + (1 << current_order);

        BuddyBlockHeader *buddy = get_block(buddy_offset);
        buddy->size_order       = current_order;
        buddy->is_free          = 1;

        list_insert(buddy_offset);

        get_block(offset)->size_order = current_order;
    }

    get_block(offset)->is_free = 0;

    // Physically commit the memory for the allocated block
    mMemory.commit(
        offset, 1ull << current_order, storage::CommitStrategy::Exact);

    // Calculate aligned payload
    const std::uint32_t base_payload_offset =
        offset + sizeof(BuddyBlockHeader) + sizeof(std::uint32_t);
    std::uint32_t aligned_payload_offset =
        (base_payload_offset + max_padding) & ~max_padding;

    // Store the block offset immediately before the aligned payload
    *mMemory.cast_view<std::uint32_t>(
        aligned_payload_offset - sizeof(std::uint32_t)) = offset;

    return { SIGNATURE, aligned_payload_offset };
}

void BuddyAllocator::deallocate(const MemoryHandle handle)
{
    if(!handle.is_valid()) return;

    USAGI_CHECK_THROW(
        LogicException,
        handle.signature == SIGNATURE,
        "Invalid MemoryHandle signature for BuddyAllocator");

    const std::uint32_t payload_offset =
        static_cast<std::uint32_t>(handle.offset);
    std::uint32_t offset = *mMemory.cast_view<const std::uint32_t>(
        payload_offset - sizeof(std::uint32_t));

    BuddyBlockHeader *block = get_block(offset);

    // Release the physical pages back to the OS
    mMemory.decommit(
        offset, 1ull << block->size_order, storage::CommitStrategy::Exact);

    block->is_free = 1;

    while(block->size_order < header()->max_order)
    {
        const std::uint32_t rel_offset = offset - header()->payload_base_offset;
        const std::uint32_t buddy_rel_offset =
            rel_offset ^ (1 << block->size_order);
        std::uint32_t buddy_offset =
            buddy_rel_offset + header()->payload_base_offset;

        if(const BuddyBlockHeader *buddy = get_block(buddy_offset);
            buddy_offset >= header()->total_size ||
            buddy->is_free == 0 ||
            buddy->size_order != block->size_order)
        {
            break;
        }

        list_remove(buddy_offset);

        offset = std::min(offset, buddy_offset);
        block  = get_block(offset);
        block->size_order++;
    }

    list_insert(offset);
}

MemoryHandle BuddyAllocator::reallocate(
    const MemoryHandle handle, const std::uint64_t new_size,
    const storage::StorageAlignment alignment)
{
    const MemoryHandle new_handle = allocate(new_size, alignment);

    if(handle.is_valid())
    {
        // To copy memory, we need to know the old size. We can derive this from
        // the old block's order.
        const std::uint32_t payload_offset =
            static_cast<std::uint32_t>(handle.offset);
        const std::uint32_t old_block_offset =
            *mMemory.cast_view<const std::uint32_t>(
                payload_offset - sizeof(std::uint32_t));

        const BuddyBlockHeader *old_block      = get_block(old_block_offset);
        // todo: warning
        const std::uint32_t     old_block_size = 1ull << old_block->size_order;

        // Approximate payload size. Note: A proper realloc would preserve
        // exactly what was used, but since we only track block capacity, we
        // copy the usable portion of the old block.
        const std::uint32_t old_payload_size =
            old_block_size - (payload_offset - old_block_offset);

        const std::uint64_t copy_size =
            std::min(new_size, static_cast<std::uint64_t>(old_payload_size));

        void       *dst = resolve(new_handle);
        const void *src = resolve(handle);
        mMemory.copy_memory(dst, src, copy_size);

        deallocate(handle);
    }

    return new_handle;
}

void *BuddyAllocator::resolve(const MemoryHandle handle) noexcept
{
    USAGI_CHECK_FATAL(
        !handle.is_valid() || handle.signature == SIGNATURE,
        "Invalid MemoryHandle signature in BuddyAllocator");
    return handle.resolve(mMemory);
}
} // namespace usagi::runtime::allocators
