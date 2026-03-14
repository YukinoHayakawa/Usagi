#include "TLSFAllocator.hpp"

#include <algorithm>

#include <Usagi/Platforms/Instructions/BitManipulations.hpp>
#include <Usagi/Runtime/Errors/Errors.hpp>

namespace usagi::runtime::allocators
{
using namespace details;

TLSFBlockHeader *TLSFAllocator::get_block(const std::uint32_t offset)
{
    if(offset == 0) return nullptr;
    return mMemory.cast_view<TLSFBlockHeader>(offset);
}

void TLSFAllocator::mapping_insert(
    const std::uint32_t size, std::uint32_t &fli, std::uint32_t &sli) const
{
    if(size < (1 << TLSF_SLI_LOG2))
    {
        fli = 0;
        sli = size;
    }
    else
    {
        fli = 31 - BitOps::count_leading_zeros(size);
        sli = (size >> (fli - TLSF_SLI_LOG2)) ^ (1 << TLSF_SLI_LOG2);
    }
}

void TLSFAllocator::list_insert(const std::uint32_t offset)
{
    TLSFBlockHeader *block = get_block(offset);
    std::uint32_t    fli, sli;
    mapping_insert(block->size(), fli, sli);

    const std::uint32_t current_head = header()->free_lists[fli][sli];
    block->next_free                 = current_head;
    block->prev_free                 = 0;

    if(current_head != 0)
    {
        get_block(current_head)->prev_free = offset;
    }

    header()->free_lists[fli][sli] = offset;
    header()->fl_bitmap |= (1 << fli);
    header()->sl_bitmap[fli] |= (1 << sli);
}

void TLSFAllocator::list_remove(const std::uint32_t offset)
{
    TLSFBlockHeader *block = get_block(offset);
    std::uint32_t    fli, sli;
    mapping_insert(block->size(), fli, sli);

    if(block->prev_free != 0)
    {
        get_block(block->prev_free)->next_free = block->next_free;
    }
    else
    {
        header()->free_lists[fli][sli] = block->next_free;
        if(header()->free_lists[fli][sli] == 0)
        {
            header()->sl_bitmap[fli] &= ~(1 << sli);
            if(header()->sl_bitmap[fli] == 0)
            {
                header()->fl_bitmap &= ~(1 << fli);
            }
        }
    }

    if(block->next_free != 0)
    {
        get_block(block->next_free)->prev_free = block->prev_free;
    }

    block->next_free = 0;
    block->prev_free = 0;
}

TLSFAllocator::TLSFAllocator(
    storage::MemoryView memory, const std::uint32_t total_size,
    const bool force_format)
    : mMemory(std::move(memory))
{
    USAGI_CHECK_THROW(
        LogicException,
        mMemory.max_size() >= total_size,
        "MemoryView is too small for requested capacity.");

    mMemory.commit(0, sizeof(TLSFHeapHeader), storage::CommitStrategy::Exact);

    if(force_format || header()->magic != TLSFHeapHeader::EXPECTED_MAGIC)
    {
        header()->magic               = TLSFHeapHeader::EXPECTED_MAGIC;
        header()->total_size          = total_size;
        header()->payload_base_offset = sizeof(TLSFHeapHeader);
        header()->fl_bitmap           = 0;

        for(std::uint32_t i = 0; i < TLSF_FLI_COUNT; ++i)
        {
            header()->sl_bitmap[i] = 0;
            for(std::uint32_t j = 0; j < TLSF_SLI_COUNT; ++j)
            {
                header()->free_lists[i][j] = 0;
            }
        }

        std::uint32_t block_size = total_size - header()->payload_base_offset;
        block_size = BitOps::align_down_pow2(block_size, 4); // Align to 4 bytes

        TLSFBlockHeader *block      = get_block(header()->payload_base_offset);
        block->prev_physical_offset = 0;
        block->size_and_flags       = block_size;
        block->set_free(true);
        block->set_prev_free(false);

        list_insert(header()->payload_base_offset);
    }
}

MemoryHandle TLSFAllocator::allocate(
    const std::uint64_t size, const storage::StorageAlignment alignment)
{
    std::uint64_t alignment_bytes = storage::to_bytes(alignment);
    alignment_bytes               = std::max<uint64_t>(alignment_bytes, 4);

    const std::uint32_t max_padding =
        static_cast<std::uint32_t>(alignment_bytes - 1);
    std::uint32_t required_size = static_cast<std::uint32_t>(size) +
        sizeof(TLSFBlockHeader) +
        sizeof(std::uint32_t) +
        max_padding;

    required_size = BitOps::align_up_pow2(required_size, 4); // 4-byte align

    std::uint32_t fli, sli;
    mapping_insert(required_size, fli, sli);

    // Filter SL bitmap to only consider slots >= sli
    std::uint32_t sl_map = header()->sl_bitmap[fli] & (~0u << sli);
    if(!sl_map)
    {
        // Filter FL bitmap to only consider classes > fli
        const std::uint32_t fl_map = header()->fl_bitmap & (~0u << (fli + 1));
        if(!fl_map)
        {
            USAGI_CHECK_THROW(
                OutOfMemoryException,
                false,
                "Out of memory blocks in TLSFAllocator"); // OOM}
        }
        fli    = BitOps::count_trailing_zeros(fl_map);
        sl_map = header()->sl_bitmap[fli];
    }

    sli = BitOps::count_trailing_zeros(sl_map);

    const std::uint32_t offset = header()->free_lists[fli][sli];
    TLSFBlockHeader    *block  = get_block(offset);
    list_remove(offset);

    if(block->size() >= required_size + sizeof(TLSFBlockHeader) + 4)
    {
        const std::uint32_t split_offset = offset + required_size;
        TLSFBlockHeader    *split        = get_block(split_offset);

        split->size_and_flags       = block->size() - required_size;
        split->prev_physical_offset = offset;
        split->set_free(true);
        split->set_prev_free(false); // We are about to mark 'block' as used

        block->set_size(required_size);

        if(const std::uint32_t next_phys_offset = split_offset + split->size();
            next_phys_offset < header()->total_size)
        {
            TLSFBlockHeader *next_phys      = get_block(next_phys_offset);
            next_phys->prev_physical_offset = split_offset;
            next_phys->set_prev_free(true);
        }

        list_insert(split_offset);
    }
    else
    {
        if(const std::uint32_t next_phys_offset = offset + block->size();
            next_phys_offset < header()->total_size)
        {
            get_block(next_phys_offset)->set_prev_free(false);
        }
    }

    block->set_free(false);

    // Calculate aligned payload
    const std::uint32_t base_payload_offset =
        offset + sizeof(TLSFBlockHeader) + sizeof(std::uint32_t);
    std::uint32_t aligned_payload_offset = BitOps::align_up_pow2(
        base_payload_offset, static_cast<std::uint32_t>(alignment_bytes));

    // Store the block offset immediately before the aligned payload
    *mMemory.cast_view<std::uint32_t>(
        aligned_payload_offset - sizeof(std::uint32_t)) = offset;

    mMemory.commit(
        offset, block->size(), storage::CommitStrategy::Aggressive64);

    return { SIGNATURE, aligned_payload_offset };
}

void TLSFAllocator::deallocate(const MemoryHandle handle)
{
    // todo: error handling
    if(!handle.is_valid()) return;

    USAGI_CHECK_THROW(
        LogicException,
        handle.signature == SIGNATURE,
        "Invalid MemoryHandle signature for TLSFAllocator");

    const std::uint32_t payload_offset =
        static_cast<std::uint32_t>(handle.offset);
    std::uint32_t offset = *mMemory.cast_view<const std::uint32_t>(
        payload_offset - sizeof(std::uint32_t));

    TLSFBlockHeader *block = get_block(offset);

    // todo: error handling
    mMemory.decommit(
        offset, block->size(), storage::CommitStrategy::Aggressive64);

    block->set_free(true);

    if(const std::uint32_t next_phys_offset = offset + block->size();
        next_phys_offset < header()->total_size)
    {
        if(TLSFBlockHeader *next_phys = get_block(next_phys_offset);
            next_phys->is_free())
        {
            list_remove(next_phys_offset);
            block->set_size(block->size() + next_phys->size());
        }
        else
        {
            next_phys->set_prev_free(true);
        }
    }

    if(block->is_prev_free() && block->prev_physical_offset != 0)
    {
        const std::uint32_t prev_offset = block->prev_physical_offset;
        TLSFBlockHeader    *prev_phys   = get_block(prev_offset);

        list_remove(prev_offset);
        prev_phys->set_size(prev_phys->size() + block->size());

        offset = prev_offset;
        block  = prev_phys;
    }

    if(const std::uint32_t final_next_phys = offset + block->size();
        final_next_phys < header()->total_size)
    {
        get_block(final_next_phys)->prev_physical_offset = offset;
    }

    list_insert(offset);
}

MemoryHandle TLSFAllocator::reallocate(
    const MemoryHandle handle, const std::uint64_t new_size,
    const storage::StorageAlignment alignment)
{
    const MemoryHandle new_handle = allocate(new_size, alignment);

    if(handle.is_valid())
    {
        const std::uint32_t payload_offset =
            static_cast<std::uint32_t>(handle.offset);
        const std::uint32_t old_block_offset =
            *mMemory.cast_view<const std::uint32_t>(
                payload_offset - sizeof(std::uint32_t));

        const TLSFBlockHeader *old_block = get_block(old_block_offset);
        const std::uint32_t    old_payload_size =
            old_block->size() - (payload_offset - old_block_offset);

        const std::uint64_t copy_size =
            std::min(new_size, static_cast<std::uint64_t>(old_payload_size));

        void       *dst = resolve(new_handle);
        const void *src = resolve(handle);
        mMemory.copy_memory(dst, src, copy_size);

        deallocate(handle);
    }

    return new_handle;
}

void *TLSFAllocator::resolve(const MemoryHandle handle) noexcept
{
    // todo: duplicated code
    USAGI_CHECK_THROW(
        LogicException,
        !handle.is_valid() || handle.signature == SIGNATURE,
        "memory handle not allocator by this kind of allocator");
    return handle.resolve(mMemory);
}
} // namespace usagi::runtime::allocators
