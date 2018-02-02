#include <cassert>
#include <algorithm>
#include <exception>

#include "CircularAllocator.hpp"

bool yuki::memory::CircularAllocator::tryAllocateFromRange(
    const std::size_t num_bytes, const std::size_t alignment,
    char *const begin, char *const end, Allocation &alloc) const
{
    assert(begin <= end);

    const auto aligned_begin = alignment ?
        reinterpret_cast<char*>(utility::roundUpUnsigned(
            reinterpret_cast<size_t>(begin),
            alignment)) :
        begin;
    const std::size_t available = end - aligned_begin;
    if(available >= num_bytes)
    {
        alloc.offset = aligned_begin - mBegin;
        alloc.length = num_bytes;
        alloc.alignment_padding = aligned_begin - begin;
        alloc.state = Allocation::State::USED;
        return true;
    }
    return false;
}

char * yuki::memory::CircularAllocator::wrapIncrement(char *original,
    std::size_t increment)
{
    original += increment;
    if(original >= mBegin + mSize)
        original -= mSize;
    return original;
}

yuki::memory::CircularAllocator::CircularAllocator(void *const begin,
    const std::size_t size)
    : mBegin { static_cast<char*>(begin) }
    , mSize { size }
{
}

void * yuki::memory::CircularAllocator::allocate(const std::size_t num_bytes,
    const std::size_t alignment)
{
    assert(num_bytes);

    Allocation alloc;

    if(mTail == mHead)
    {
        if(!mAllocations.empty()) // buffer is full
            throw std::bad_alloc();

        // otherwise the buffer is empty, reset pointers
        mTail = mHead = mBegin;

        // since the buffer is empty, allocate from full range
        if(tryAllocateFromRange(
            num_bytes,
            alignment,
            mBegin,
            mBegin + mSize,
            alloc))
        {
            mHead = wrapIncrement(mHead, alloc.alignment_padding + alloc.length);
            mAllocations.push_back(alloc);
        }
    }
    else if(mHead > mTail) // space between head and tail is allocated
    {
        // allocate from space after head
        if(tryAllocateFromRange(
            num_bytes,
            alignment,
            mHead,
            mBegin + mSize,
            alloc))
        {
            mHead = wrapIncrement(mHead, alloc.alignment_padding + alloc.length);
            mAllocations.push_back(alloc);
        }
        // allocate from space before tail
        else if(tryAllocateFromRange(
            num_bytes,
            alignment,
            mBegin,
            mTail,
            alloc))
        {
            Allocation padding;
            padding.offset = mHead - mBegin;
            padding.state = Allocation::State::PENDING_FREE;
            padding.length = mBegin + mSize - mHead;
            mAllocations.push_back(padding);

            mHead = mBegin + alloc.offset + alloc.length;
            mAllocations.push_back(alloc);
        }
    }
    else
    {
        if(tryAllocateFromRange(
            num_bytes,
            alignment,
            mHead,
            mTail,
            alloc))
        {
            mHead = wrapIncrement(mHead, alloc.alignment_padding + alloc.length);
            mAllocations.push_back(alloc);
        }
    }
    if(alloc.state != Allocation::State::USED)
        throw std::bad_alloc();

    return mBegin + alloc.offset;
}

void yuki::memory::CircularAllocator::deallocate(void *pointer)
{
    assert(!mAllocations.empty());

    auto iter = mAllocations.begin();

    if(mBegin + iter->offset == pointer)
    {
        iter->state = Allocation::State::PENDING_FREE;

        while(iter != mAllocations.end() && iter->state == Allocation::State::PENDING_FREE)
        {
            mTail = mBegin + iter->offset + iter->length;
            if(mTail >= mBegin + mSize) mTail -= mSize;

            mAllocations.pop_front();
            iter = mAllocations.begin();
        }
    }
    else
    {
        iter = std::find_if(mAllocations.begin(), mAllocations.end(), [=](const Allocation &alloc) {
            return mBegin + alloc.offset == pointer;
        });
        if(iter == mAllocations.end())
            throw std::invalid_argument("the pointer is not allocated from this allocator");

        assert(iter->state == Allocation::State::USED);
        iter->state = Allocation::State::PENDING_FREE;
    }
}
