#include "CircularAllocator.hpp"

#include <cassert>

#include <Usagi/Runtime/ErrorHandling.hpp>

namespace usagi
{
CircularAllocator::SegmentIterator::SegmentIterator(
    CircularAllocator *allocator,
    Segment *segment)
    : mAllocator(allocator)
    , mSegment(segment)
{
    assert(mSegment <= mAllocator->mMemory.end());
}

// ReSharper disable once CppMemberFunctionMayBeConst
bool CircularAllocator::SegmentIterator::try_merge_next()
{
    auto it = next();
    if(it == mAllocator->end()) return false;
    if(!it->freed) return false;
    mSegment->length += it->length;
    return true;
}

bool CircularAllocator::SegmentIterator::can_allocate(
    const std::size_t size) const
{
    return mSegment->freed && mSegment->length >= size + sizeof(Segment);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
CircularAllocator::Segment * CircularAllocator::from_payload(void *ptr)
{
    return static_cast<Segment *>(raw_advance(
        ptr,
        -static_cast<std::ptrdiff_t>(sizeof(Segment))
    ));
}

CircularAllocator::SegmentIterator CircularAllocator::head()
{
    return { this, as_segment(mHead) };
}

CircularAllocator::SegmentIterator CircularAllocator::end()
{
    return { this, as_segment(mMemory.size()) };
}

bool CircularAllocator::can_tail_allocate(const size_t size) const
{
    const auto end_point = mTail >= mHead ? mMemory.size() : mHead;
    return mTail + Segment::make_segment_size(size) <= end_point;
}

void * CircularAllocator::commit_tail_allocate(const size_t size)
{
    Segment *segment = as_segment(mTail);

    segment->freed = false;
    segment->length = Segment::make_segment_size(size);

    mTail += segment->length;

    // free space won't allow another allocation, include these bytes in this
    // allocation.
    if(mTail + sizeof(Segment) >= mMemory.size())
    {
        segment->length += mMemory.size() - mTail;
        mTail = 0;
    }

    return segment->payload();
}

// todo initial state?
void CircularAllocator::merge_head_segments()
{
    SegmentIterator it = head();
    while(it.try_merge_next()) { }
    if(it->freed)
        mHead += it->length;
    // todo: some space may be left at the end. in that case head won't wrap
    if(mHead == mMemory.size())
        mHead = 0;
}

void CircularAllocator::init_segment()
{
    assert(!mHead && !mTail);
    mHead = init_free_segment(0, mMemory.size());
}

// bug
bool CircularAllocator::can_head_allocate(const size_t size)
{
    return head()->length >= size;
}

void CircularAllocator::wrap_back()
{
    mTail = 0;
}

bool CircularAllocator::allocated_by_us(void *ptr) const
{
    // todo: can't ensure the ptr actually points to a segment
    return mMemory.contains(ptr);
}

void CircularAllocator::free_segment(void *ptr)
{
    Segment *segment = from_payload(ptr);
    assert(segment->freed == false);
    segment->freed = true;
}

CircularAllocator::CircularAllocator(const MemoryView memory)
    : mMemory(memory)
{
}

void * CircularAllocator::allocate(const std::size_t size)
{
    USAGI_ASSERT_THROW(size, std::bad_alloc());

    merge_head_segments();
    if(can_tail_allocate(size))
    {
        return commit_tail_allocate(size);
    }
    if(can_head_allocate(size))
    {
        wrap_back();
        return commit_tail_allocate(size);
    }
    USAGI_THROW(std::bad_alloc());
}

void CircularAllocator::deallocate(void *ptr)
{
    if(!allocated_by_us(ptr))
        USAGI_THROW(std::bad_alloc());
    free_segment(ptr);
}
}
