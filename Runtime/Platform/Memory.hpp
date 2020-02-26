#pragma once

#include <string_view>

#include <Usagi/Runtime/Memory/Region.hpp>

// These functions are not defined in the core library and must be linked
// with runtime implementation library for the targeted platform.

namespace usagi::platform::memory
{
// ========================== Virtual Memory ================================ //

// Get the page size in bytes.
std::size_t page_size();
std::size_t round_up_to_page_size(std::size_t size_bytes);

// Reserve a virtual address range. If commit is true, the range is also
// committed.
// Throws std::bad_alloc if the allocation fails.
// Returns the actual allocated memory region.
 [[nodiscard]] // Raw resource must be properly managed.
MemoryRegion allocate(std::size_t size_bytes, bool commit);

// Commit a range of virtual address. The [ptr, ptr + size_bytes) must
// be in a valid range returned by allocate().
// Returns the actual affected memory region.
MemoryRegion commit(void *ptr, std::size_t size_bytes);

// Decommit a range of virtual address. The [ptr, ptr + size_bytes) must
// be in a valid range returned by allocate().
// Returns the actual affected memory region.
MemoryRegion decommit(void *ptr, std::size_t size_bytes);

// Free a range of virtual address. The [ptr, ptr + size_bytes) must be
// exactly a valid range returned by virtualMemoryAllocate(). The range will be
// decommitted and no longer accessible.
// Returns the actual affected memory region.
MemoryRegion free(void *ptr, std::size_t size_bytes);
//
// // Lock a region of memory in the working set so it is resident in the physical
// // memory.
// // Returns the actual affected memory region.
// MemoryRegion lock(void *ptr, std::size_t size_bytes);
//
// // Unlock a region of memory.
// // Returns the actual affected memory region.
// MemoryRegion unlock(void *ptr, std::size_t size_bytes);

// Mark a region of memory as no longer in use and can be used by the virtual
// memory manager for other purposes.
// Returns the actual affected memory region.
MemoryRegion offer(void *ptr, std::size_t size_bytes);

// ========================== Memory Mapping ================================ //

// Inform the operating system of the memory region going to be used soon
// to improve performance.
// Basically madvise(MADV_WILLNEED) for POSIX or PrefetchVirtualMemory for
// Win32.
void prefetch(void *ptr, std::size_t size_bytes);

// Returns the actual affected memory region.
MemoryRegion flush(void *ptr, std::size_t size_bytes);
}
