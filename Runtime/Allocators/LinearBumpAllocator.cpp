#include "LinearBumpAllocator.hpp"

#include <bit>
#include <cassert>

#include <Usagi/Runtime/Errors/Exceptions.hpp>

namespace usagi::runtime::allocators
{
LinearBumpAllocator::LinearBumpAllocator(
    MemoryView memory, const bool force_format)
    : mMemory(std::move(memory))
{
    // The allocator needs to commit at least the first block to house the
    // header
    mMemory.commit(0, sizeof(LinearBumpHeapHeader));

    if(force_format || header()->magic != LinearBumpHeapHeader::EXPECTED_MAGIC)
    {
        header()->magic = LinearBumpHeapHeader::EXPECTED_MAGIC;

        // Payload base immediately follows the header, aligned to 64 bytes
        constexpr std::uint64_t payload_base =
            (sizeof(LinearBumpHeapHeader) + 63) & ~63ull;

        header()->current_offset.store(payload_base, std::memory_order_relaxed);
        header()->committed_offset.store(
            sizeof(LinearBumpHeapHeader), std::memory_order_relaxed);
    }
}

MemoryHandle LinearBumpAllocator::allocate(
    const std::uint64_t size, const storage::StorageAlignment alignment)
{
    // Shio: Wait-free bump! We use a CAS loop because alignment means the
    // increment isn't constant.
    std::uint64_t current =
        header()->current_offset.load(std::memory_order_relaxed);
    const std::uint64_t alignment_bytes = storage::to_bytes(alignment);
    std::uint64_t       aligned_offset;
    std::uint64_t       new_offset;

    do
    {
        aligned_offset =
            (current + alignment_bytes - 1) & ~(alignment_bytes - 1);
        new_offset = aligned_offset + size;

        if(new_offset > mMemory.max_size())
        {
            USAGI_THROW(std::bad_alloc()); // Exceeded maximum staging capacity
        }
    }
    while(!header()->current_offset.compare_exchange_weak(current,
        new_offset,
        std::memory_order_relaxed,
        std::memory_order_relaxed));

    // Shio: Check if we need to commit more physical OS pages.
    // We use a simple CAS loop to ensure only one thread triggers the OS page
    // fault at a time.
    std::uint64_t current_committed =
        header()->committed_offset.load(std::memory_order_acquire);
    while(new_offset > current_committed)
    {
        // Align commit boundary to 4KB (standard page) for the watermark.
        // MemoryView's sys implementation will handle actual OS allocation
        // granularity.

        if(const std::uint64_t desired_commit =
                (new_offset + 4'095) & ~4'095ull;
            header()->committed_offset.compare_exchange_weak(current_committed,
                desired_commit,
                std::memory_order_release,
                std::memory_order_acquire))
        {
            // Only the thread that successfully updates the watermark talks to
            // the OS.
            const std::uint64_t size_to_commit =
                desired_commit - current_committed;
            mMemory.commit(current_committed, size_to_commit);
            break;
        }
        // If CAS failed, another thread already updated current_committed. Loop
        // and check again.
    }

    return { SIGNATURE, aligned_offset };
}

void LinearBumpAllocator::deallocate([[maybe_unused]] MemoryHandle handle)
{
    // No-op for bump allocators.
}

void LinearBumpAllocator::reset()
{
    constexpr std::uint64_t payload_base =
        (sizeof(LinearBumpHeapHeader) + 63) & ~63ull;
    header()->current_offset.store(payload_base, std::memory_order_relaxed);
}

std::uint64_t LinearBumpAllocator::active_bytes() const noexcept
{
    constexpr std::uint64_t payload_base =
        (sizeof(LinearBumpHeapHeader) + 63) & ~63ull;
    return header()->current_offset.load(std::memory_order_relaxed) -
        payload_base;
}

void *LinearBumpAllocator::resolve(const MemoryHandle handle) const noexcept
{
    assert(!handle.is_valid() || handle.signature == SIGNATURE);
    return handle.resolve(mMemory);
}
} // namespace usagi::runtime::allocators
