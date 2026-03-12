#pragma once

#include <cstdint>

namespace usagi::runtime::storage
{
/**
 * Shio:
 * Defines the strategy for committing physical pages to a virtual address
 * space. This is used by the VirtualPageManager to optimize OS syscalls based
 * on the allocator's access patterns.
 */
enum class CommitStrategy : std::uint8_t
{
    /**
     * Commits exactly the requested pages.
     * Best for large or sparse allocations (e.g., BuddyAllocator) where
     * committing extra pages would waste physical RAM.
     */
    Exact,

    /**
     * Commits pages in 64-page batches (e.g., 256KB chunks) if any page in the
     * batch is touched.
     * Best for highly fragmented/dynamic allocators (e.g., TLSFAllocator) to
     * minimize OS syscalls.
     */
    Aggressive64,

    /**
     * Linearly commits pages based on a high-water mark.
     * Best for staging/bump allocators (e.g., LinearBumpAllocator).
     */
    Watermark,

    /**
     * Bypasses the VirtualPageManager and directly talks to the OS.
     * WARNING: Only the VirtualPageManager or manual staging buffers should use
     * this strategy.
     */
    Bypass,
};
} // namespace usagi::runtime::storage
