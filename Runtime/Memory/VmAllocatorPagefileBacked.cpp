﻿#include "VmAllocatorPagefileBacked.hpp"

#include <stdexcept>
#include <cassert>

#include <Usagi/Runtime/Platform/Memory.hpp>
#include <Usagi/Runtime/ErrorHandling.hpp>

namespace usagi
{
using namespace platform;

// Strong exception guarantee
std::size_t VmAllocatorPagefileBacked::round_up_allocation_size_checked(
    const std::size_t size_bytes) const
{
    const auto new_committed_bytes =
        memory::round_up_to_page_size(size_bytes);

    // If the requested size is larger than reserved size, fail.
    if(new_committed_bytes > mReservedBytes)
        USAGI_THROW(std::bad_alloc());

    return new_committed_bytes;
}

void VmAllocatorPagefileBacked::assert_allocation_happened() const
{
    // There is no way of knowing the base address except calling allocate().
    // Thus allocation must happened at least once.
    assert(mCommittedBytes > 0);
}

void VmAllocatorPagefileBacked::check_positive_size(const std::size_t size)
{
    if(size == 0)
        USAGI_THROW(std::bad_alloc());
}

void VmAllocatorPagefileBacked::check_allocated_by_us(void *ptr) const
{
    if(ptr != mBaseAddress)
        USAGI_THROW(std::bad_alloc());
}

VmAllocatorPagefileBacked::~VmAllocatorPagefileBacked()
{
    // Ensure correct behavior of moved instances.
    if(mBaseAddress)
        memory::free(mBaseAddress, mReservedBytes);
    mReservedBytes = 0;
    mCommittedBytes = 0;
}

VmAllocatorPagefileBacked::VmAllocatorPagefileBacked(
    VmAllocatorPagefileBacked &&other) noexcept
    : mBaseAddress(other.mBaseAddress)
    , mReservedBytes(other.mReservedBytes)
    , mCommittedBytes(other.mCommittedBytes)
{
    other.mBaseAddress = nullptr;
    // Ensure the correct behaviors of allocation functions on the moved
    // object
    other.mReservedBytes = 0;
    other.mCommittedBytes = 0;
}

void VmAllocatorPagefileBacked::reserve(const std::size_t size_bytes)
{
    if(mBaseAddress)
        USAGI_THROW(std::runtime_error("Double initialization"));

    const auto allocation = memory::allocate(size_bytes, false);
    mBaseAddress = static_cast<char *>(allocation.base_address);
    mReservedBytes = allocation.length;
}

// Strong exception guarantee
void * VmAllocatorPagefileBacked::allocate(const std::size_t size)
{
    check_positive_size(size);

    // If an allocation exists, fail all subsequent allocations.
    if(mCommittedBytes > 0)
        USAGI_THROW(std::bad_alloc());

    if(!mBaseAddress) reserve(memory::round_up_to_page_size(size));

    const auto new_committed_bytes =
        round_up_allocation_size_checked(size);

    // Otherwise, commit the range of used memory and return the base address.
    memory::commit(mBaseAddress, new_committed_bytes);

    // Commit changes to this object after successful operation
    mCommittedBytes = new_committed_bytes;

    return mBaseAddress;
}

// Strong exception guarantee
void * VmAllocatorPagefileBacked::reallocate(
    void *old_ptr,
    const std::size_t new_size)
{
    if(old_ptr == nullptr)
        return allocate(new_size);

    check_positive_size(new_size);

    const auto new_committed_bytes =
        round_up_allocation_size_checked(new_size);

    // No change in amount of committed pages
    if(new_committed_bytes == mCommittedBytes)
        return mBaseAddress;

    // More pages to be committed
    if(new_committed_bytes > mCommittedBytes)
    {
        const auto distance =
            new_committed_bytes - mCommittedBytes;
        memory::commit(mBaseAddress + mCommittedBytes, distance);
    }
    // Release tail pages
    else
    {
        const auto distance =
            mCommittedBytes - new_committed_bytes;
        memory::decommit(mBaseAddress + new_committed_bytes, distance);
    }

    // Commit changes to this object after successful operation
    mCommittedBytes = new_committed_bytes;
    return mBaseAddress;
}

// Strong exception guarantee
void VmAllocatorPagefileBacked::deallocate(void *ptr)
{
    check_allocated_by_us(ptr);
    assert_allocation_happened();

    memory::decommit(mBaseAddress, mCommittedBytes);
    mCommittedBytes = 0;
}
}