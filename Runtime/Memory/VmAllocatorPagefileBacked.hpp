#pragma once

#include <cstddef>
#include <cstdint>

#include <Usagi/Concept/Allocator/ReallocatableAllocator.hpp>
#include <Usagi/Library/Memory/Noncopyable.hpp>

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
class VmAllocatorPagefileBacked : Noncopyable
{
    char *mBaseAddress = nullptr;
    std::size_t mReservedBytes = 0;
    std::size_t mCommittedBytes = 0;

    std::size_t round_up_allocation_size_checked(std::size_t size_bytes) const;
    void assert_allocation_happened() const;
    static void check_positive_size(std::size_t size);
    void check_allocated_by_us(void *ptr) const;
    void check_boundary(std::uint64_t size) const;

public:
    // Reserve 16GiB by default
    explicit VmAllocatorPagefileBacked(std::size_t reserve_size = 1ull << 34);
    VmAllocatorPagefileBacked(VmAllocatorPagefileBacked &&other) noexcept;
    ~VmAllocatorPagefileBacked();

    using value_type = void;

    void reserve(std::size_t size_bytes);

    // Move assignment and copy ctor & assignment implicitly deleted.

    void * allocate(std::size_t size);
    void * reallocate(void *old_ptr, std::size_t new_size);
    void deallocate(void *ptr);

    /**
     * \brief Use zero pages to replace the content of specified range of
     * memory. Non-page-aligned parts will be zeroed using `memset()`
     * If `size == 0`, the whole allocated heap will be zeroed.
     * \param ptr Must be the address returned by `allocate()` or `reallocate()`
     */
    void zero_memory(void *ptr, std::uint64_t offset = 0, std::size_t size = 0);

    void * base_address() const { return mBaseAddress; }
    std::size_t reserved_bytes() const { return mReservedBytes; }
    std::size_t committed_bytes() const { return mCommittedBytes; }

    std::size_t max_size() const noexcept { return mReservedBytes; }
};
static_assert(ReallocatableAllocator<VmAllocatorPagefileBacked>);
}
