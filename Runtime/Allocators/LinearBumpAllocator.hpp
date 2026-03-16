#pragma once

#include <atomic>

#include <Usagi/Platforms/PlatformTraits.hpp>
#include <Usagi/Runtime/Allocators/Concepts/VariableSizeAllocator.hpp>
#include <Usagi/Runtime/Allocators/Concepts/WaitFreeAllocator.hpp>
#include <Usagi/Runtime/Storage/Views/MemoryView.hpp>

#include "AllocatorCommon.hpp"

namespace usagi::runtime::allocators
{
namespace details
{
/**
 * \brief The header mapped at the base of the linear bump allocator's memory
 * region.
 */
struct LinearBumpHeapHeader
{
    static constexpr std::uint64_t EXPECTED_MAGIC =
        0x4C49'4E45'4152'0000; // "LINEAR\0\0"

    std::uint64_t magic;

    // Ensure the atomic counter sits on its own cache line to prevent false
    // sharing
    alignas(platforms::PlatformTraits::cpu_cache_line_size())
        std::atomic<std::uint64_t> current_offset;

    // Tracks how much of the virtual reservation is currently backed by
    // physical memory
    alignas(platforms::PlatformTraits::cpu_cache_line_size())
        std::atomic<std::uint64_t> committed_offset;
};
} // namespace details

/**
 * \brief A wait-free, thread-safe linear bump allocator backed by MemoryView.
 *
 * Shio:
 * Designed to be used as a thread-local or work-group-local Staging Area.
 * Systems allocate raw byte chunks into this area without any mutex locking.
 * The memory view dynamically commits physical OS pages as the logical offset
 * expands.
 *
 * **Use Cases:**
 * - **Frame-local / Transient Data:** Ideal for allocations that only exist for
 * a single frame (e.g., render commands, physics contacts, temporary event
 * queues).
 * - **Wait-Free Staging:** Because it only uses
 * `std::atomic<uint64_t>::fetch_add`, multiple worker threads can allocate
 * chunks concurrently without ever blocking each other.
 * - **Zero Fragmentation:** Bumps forward perfectly sequentially. Deallocation
 * is a single `reset()` call that drops the entire arena instantly.
 */
class LinearBumpAllocator
{
    storage::MemoryView mMemory;

    [[nodiscard]]
    details::LinearBumpHeapHeader *header() noexcept
    {
        return mMemory.cast_view<details::LinearBumpHeapHeader>();
    }

public:
    static constexpr std::uint8_t SIGNATURE =
        allocator_signature(AllocatorType::Linear);
    static constexpr bool IS_WAIT_FREE = true;

    explicit LinearBumpAllocator(
        storage::MemoryView memory, bool force_format = false);

    [[nodiscard]]
    MemoryHandle allocate(
        std::uint64_t size, storage::StorageAlignment alignment);

    /**
     * \brief No-op. Bump allocators cannot free individual allocations.
     */
    void deallocate(MemoryHandle handle);

    /**
     * \brief Flushes the staging area, making it ready for the next
     * frame/phase. Does not decommit OS memory, keeping it "warm" for maximum
     * speed.
     */
    void reset();

    [[nodiscard]]
    std::uint64_t active_bytes() const noexcept;

    [[nodiscard]]
    void *resolve(MemoryHandle handle) noexcept;

    [[nodiscard]]
    const storage::MemoryView &view() const noexcept
    {
        return mMemory;
    }
};

static_assert(VariableSizeAllocator<LinearBumpAllocator>);
static_assert(WaitFreeAllocator<LinearBumpAllocator>);
} // namespace usagi::runtime::allocators
