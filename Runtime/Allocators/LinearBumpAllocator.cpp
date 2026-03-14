#include "LinearBumpAllocator.hpp"

#include <Usagi/Platforms/Instructions/BitManipulations.hpp>
#include <Usagi/Runtime/Errors/Errors.hpp>

namespace usagi::runtime::allocators
{
using namespace details;

using BitOps = platforms::instructions::
    DefaultBitManipulationInstructions<OperandBitWidth::_64>;

LinearBumpAllocator::LinearBumpAllocator(
    storage::MemoryView memory, const bool force_format)
    : mMemory(std::move(memory))
{
    // The allocator needs to commit at least the first block to house the
    // header
    mMemory.commit(
        0, sizeof(LinearBumpHeapHeader), storage::CommitStrategy::Bypass);

    if(force_format || header()->magic != LinearBumpHeapHeader::EXPECTED_MAGIC)
    {
        header()->magic = LinearBumpHeapHeader::EXPECTED_MAGIC;

        // Payload base immediately follows the header, aligned to the hardware
        // cache line
        constexpr std::uint64_t payload_base = BitOps::align_up_pow2(
            sizeof(LinearBumpHeapHeader),
            platforms::PlatformTraits::cpu_cache_line_size());

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
        aligned_offset = BitOps::align_up_pow2(current, alignment_bytes);
        new_offset     = aligned_offset + size;

        // Exceeded maximum staging capacity
        USAGI_CHECK_THROW(
            OutOfMemoryException, new_offset <= mMemory.max_size());
    }
    while(!header()->current_offset.compare_exchange_weak(
        current,
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
        // Align commit boundary to standard system page size for the watermark.
        // MemoryView's sys implementation will handle actual OS allocation
        // granularity.
        constexpr std::uint64_t page_size =
            storage::to_bytes(storage::StoragePageSize::Page_4KB);

        if(const std::uint64_t desired_commit =
                BitOps::align_up_pow2(new_offset, page_size);
            header()->committed_offset.compare_exchange_weak(
                current_committed,
                desired_commit,
                std::memory_order_release,
                std::memory_order_acquire))
        {
            // Only the thread that successfully updates the watermark talks to
            // the OS.
            const std::uint64_t size_to_commit =
                desired_commit - current_committed;
            mMemory.commit(
                current_committed,
                size_to_commit,
                storage::CommitStrategy::Bypass);
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
    constexpr std::uint64_t payload_base = BitOps::align_up_pow2(
        sizeof(LinearBumpHeapHeader),
        platforms::PlatformTraits::cpu_cache_line_size());

    // Shio: Maintain a fixed warm committed range (e.g., 16MB) to avoid
    // frequent OS faults, but shrink anything beyond that to prevent memory
    // bloat after spike frames.
    constexpr std::uint64_t warm_size =
        storage::to_bytes(storage::StoragePageSize::Page_16MB);
    const std::uint64_t current_committed =
        header()->committed_offset.load(std::memory_order_acquire);

    if(current_committed > payload_base + warm_size)
    {
        constexpr std::uint64_t target_commit = payload_base + warm_size;
        mMemory.decommit(
            target_commit,
            current_committed - target_commit,
            storage::CommitStrategy::Bypass);
        header()->committed_offset.store(
            target_commit, std::memory_order_release);
    }

    header()->current_offset.store(payload_base, std::memory_order_relaxed);
}

std::uint64_t LinearBumpAllocator::active_bytes() const noexcept
{
    constexpr std::uint64_t payload_base = BitOps::align_up_pow2(
        sizeof(LinearBumpHeapHeader),
        platforms::PlatformTraits::cpu_cache_line_size());

    return const_cast<LinearBumpAllocator *>(this)
               ->header()
               ->current_offset.load(std::memory_order_relaxed) -
        payload_base;
}

void *LinearBumpAllocator::resolve(const MemoryHandle handle) noexcept
{
    // todo: duplicated code
    USAGI_CHECK_THROW(
        LogicException,
        !handle.is_valid() || handle.signature == SIGNATURE,
        "memory handle not allocator by this kind of allocator");
    return handle.resolve(mMemory);
}
} // namespace usagi::runtime::allocators
