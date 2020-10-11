#pragma once

#include <cstddef>

#include <Usagi/Concept/Allocator/ReallocatableAllocator.hpp>

namespace usagi
{
/**
 * \brief Use the virtual memory system calls provided by the operating system
 * to allocate memory. This allocator reserves a range of virtual addresses
 * when constructed and frees the reservation during destruction. It always
 * returns the same address, which is the first address of the reserved address
 * range, when allocation is requested.
 *
 * The virtual memory covered by the address range is initially only reserved
 * but not committed. The first call to allocate() causes the allocator to
 * commit the range of addresses starting from the first address to the last
 * page covering the requested size. It then returns the beginning of the
 * address range.
 *
 * Subsequent calls to allocate() fails with nullptr being returned until
 * deallocate() is successfully called with the originally returned pointer.
 *
 * reallocate() adjusts the size of committed range starting from the base
 * address. If reallocate() is used with nullptr, it behaves like allocate().
 *
 * For the details, see the corresponding member functions.
 *
 * The actual calls to the operating system are platform-dependent and only
 * declared in the `platform` namespace. Appropriate implementation must be
 * linked in order to compile the program.
 *
 * This may be implemented on Windows with VirtualAlloc, and mmap family
 * of functions on Linux.
 *
 * Since the virtual address will not change after reserving, this allocator
 * can be used to implement dynamic vectors with zero expanding cost if the
 * maximum allocation size is known.
 */
class VmAllocatorPagefileBacked
{
    char *mBaseAddress = nullptr;
    std::size_t mReservedBytes = 0;
    std::size_t mCommittedBytes = 0;

    std::size_t round_up_allocation_size_checked(std::size_t size_bytes) const;
    void assert_allocation_happened() const;
    static void check_positive_size(std::size_t size);
    void check_allocated_by_us(void *ptr) const;


public:
    VmAllocatorPagefileBacked() = default;
    ~VmAllocatorPagefileBacked();

    VmAllocatorPagefileBacked(VmAllocatorPagefileBacked &&other) noexcept;

    void reserve(std::size_t size_bytes);

    // Move assignment and copy ctor & assignment implicitly deleted.

    void * allocate(std::size_t size);
    void * reallocate(void *old_ptr, std::size_t new_size);
    void deallocate(void *ptr);

    std::size_t max_size() const noexcept
    {
        return mReservedBytes;
    }
};
static_assert(ReallocatableAllocator<VmAllocatorPagefileBacked>);
}
