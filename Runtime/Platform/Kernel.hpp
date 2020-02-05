#pragma once

#include <cstddef>

// These functions are not defined in the core library and must be linked
// with runtime implementation library for the targeted platform.

namespace usagi::platform::kernel
{
namespace virtual_memory
{
// Get the page size in bytes.
std::size_t page_size();
std::size_t round_up_to_page_size(std::size_t size_bytes);

struct Allocation
{
    void *base_address = nullptr;
    std::size_t length = 0;
};

// Reserve a virtual address range. If commit is true, the range is also
// committed.
// Throws std::bad_alloc if the allocation fails.
Allocation allocate(std::size_t size_bytes, bool commit);

// Commit a range of virtual address. The [ptr, ptr + size_bytes) must
// be in a valid range returned by allocate().
void commit(void *ptr, std::size_t size_bytes);

// Decommit a range of virtual address. The [ptr, ptr + size_bytes) must
// be in a valid range returned by allocate().
void decommit(void *ptr, std::size_t size_bytes);

// Free a range of virtual address. The [ptr, ptr + size_bytes) must be
// exactly a valid range returned by virtualMemoryAllocate(). The range will be
// decommitted and no longer accessible.
void free(void *ptr, std::size_t size_bytes);
}
}
