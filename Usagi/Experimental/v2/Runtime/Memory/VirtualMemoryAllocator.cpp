#include "VirtualMemoryAllocator.hpp"

#include <stdexcept>
#include <cassert>

#include <Usagi/Experimental/v2/Runtime/Platform/Kernel.hpp>
#include <Usagi/Core/Exception.hpp>

namespace usagi
{
using namespace platform::kernel;

std::size_t VirtualMemoryAllocator::round_up_allocation_size_checked(
    std::size_t size_bytes) const
{
    const auto new_committed_bytes =
        virtual_memory::round_up_to_page_size(size_bytes);

    // If the requested size is larger than reserved size, fail.
    if(new_committed_bytes > mReservedBytes)
        USAGI_THROW(std::bad_alloc());

    return new_committed_bytes;
}

void VirtualMemoryAllocator::assert_allocation_happened() const
{
    // There is no way of knowing the base address except calling allocate().
    // Thus allocation must happened at least once.
    assert(mCommittedBytes > 0);
}

VirtualMemoryAllocator::VirtualMemoryAllocator(const std::size_t reserved_bytes)
{
    const auto allocation = virtual_memory::allocate(mReservedBytes, false);
    mBaseAddress = static_cast<char *>(allocation.base_address);
    mReservedBytes = allocation.length;
}

VirtualMemoryAllocator::~VirtualMemoryAllocator()
{
    if(mBaseAddress)
        virtual_memory::free(mBaseAddress, mReservedBytes);
}

void * VirtualMemoryAllocator::allocate(const std::size_t size)
{
    // If an allocation exists, fail all subsequent allocations.
    if(mCommittedBytes > 0)
        USAGI_THROW(std::bad_alloc());

    const auto new_committed_bytes =
        round_up_allocation_size_checked(size);

    // Otherwise, commit the range of used memory and return the base address.
    virtual_memory::commit(mBaseAddress, new_committed_bytes);
    mCommittedBytes = new_committed_bytes;

    return mBaseAddress;
}

void * VirtualMemoryAllocator::reallocate(void *old_ptr, std::size_t new_size)
{
    if(old_ptr != mBaseAddress)
        USAGI_THROW(std::bad_alloc());

    assert_allocation_happened();

    const auto new_committed_bytes =
        round_up_allocation_size_checked(new_size);

    if(new_committed_bytes == mCommittedBytes)
        return mBaseAddress;

    // More pages to be committed
    if(new_committed_bytes > mCommittedBytes)
    {
        const auto distance =
            new_committed_bytes - mCommittedBytes;
        virtual_memory::commit(mBaseAddress + mCommittedBytes, distance);
    }
    // Release tail pages
    else
    {
        const auto distance =
            mCommittedBytes - new_committed_bytes;
        virtual_memory::decommit(mBaseAddress + new_committed_bytes, distance);
    }
    mCommittedBytes = new_committed_bytes;
    return mBaseAddress;
}

void VirtualMemoryAllocator::deallocate(void *ptr)
{
    if(ptr != mBaseAddress)
        USAGI_THROW(std::bad_alloc());

    assert_allocation_happened();

    virtual_memory::decommit(mBaseAddress, mCommittedBytes);
    mCommittedBytes = 0;
}

std::size_t VirtualMemoryAllocator::max_size() const
{
    return mReservedBytes;
}
}
