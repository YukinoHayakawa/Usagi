#include "CircularAllocator.hpp"

#include <cassert>
#include <iostream>

#include <Usagi/Runtime/ErrorHandling.hpp>
#include <Usagi/Library/Memory/Arithmetic.hpp>

namespace usagi
{
void * CircularAllocator::Segment::payload()
{
    // payload is right after the header
    return raw_advance(this, sizeof(Segment));
}

bool CircularAllocator::Segment::can_allocate(const std::size_t size) const
{
    if(!freed) return false;
    // don't have to leave the space for extra segment header
    return length >= sizeof(Segment) + size;
}

std::size_t CircularAllocator::Segment::do_allocate(const std::size_t size)
{
    assert(can_allocate(size));

    // space usable for current allocation
    const std::size_t available = length - sizeof(Segment);
    // space left for next allocation
    std::size_t leftover = available - size;
    // check for underflow
    assert(leftover < length);

    // if no more space is enough for next alloc, eat it all
    if(leftover < 1 + sizeof(Segment))
        leftover = 0;

    freed = 0;
    length -= leftover;

    assert(length > 0);

    return leftover;
}

CircularAllocator::SegmentIterator::SegmentIterator(
    CircularAllocator *allocator,
    Segment *segment)
    : mAllocator(allocator)
    , mSegment(segment)
{
    assert(mSegment <= mAllocator->mMemory.end());
}

CircularAllocator::SegmentIterator
CircularAllocator::SegmentIterator::seq_next() const
{
    return SegmentIterator(
        mAllocator,
        raw_advance(mSegment, mSegment->length)
    );
}

CircularAllocator::SegmentIterator
CircularAllocator::SegmentIterator::linked_next() const
{
    auto next = seq_next();
    if(next == mAllocator->end()) next = mAllocator->begin();
    return next;
}

void CircularAllocator::SegmentIterator::commit_possible_head_advance() const
{
    if(*this == mAllocator->head() && mSegment->freed)
        mAllocator->head_increment(mSegment->length);
}

bool CircularAllocator::SegmentIterator::can_allocate(
    const std::size_t size) const
{
    return mSegment->freed && mSegment->length >= size + sizeof(Segment);
}

// ReSharper disable once CppMemberFunctionMayBeConst
CircularAllocator::Segment * CircularAllocator::as_segment(
    const std::size_t offset)
{
    return raw_advance(mMemory.as_mutable<Segment>(), offset);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
CircularAllocator::Segment * CircularAllocator::segment_from_payload(void *ptr)
{
    return static_cast<Segment *>(raw_advance(
        ptr,
        -static_cast<std::ptrdiff_t>(sizeof(Segment))
    ));
}

std::size_t CircularAllocator::offset_from_segment(const Segment *segment) const
{
    return reinterpret_cast<std::size_t>(segment) -
        reinterpret_cast<std::size_t>(mMemory.base_address());
}

CircularAllocator::SegmentIterator CircularAllocator::at(
    const std::size_t offset)
{
    return { this, as_segment(offset) };
}

CircularAllocator::Segment * CircularAllocator::init_free_segment(
    const std::size_t offset,
    const std::size_t length)
{
    assert(length >= sizeof(Segment));
    Segment * segment = as_segment(offset);
    segment->freed = 1;
    segment->length = length;
    return segment;
}

// implements invariant 2.
void CircularAllocator::init()
{
    mHead = mTail = 0;
    init_free_segment(0, mMemory.size());
}

bool CircularAllocator::allocated_by_us(void *ptr) const
{
    // todo: can't ensure the ptr actually points to a segment
    return mMemory.contains(ptr);
}

CircularAllocator::CircularAllocator(const MemoryView memory)
    : mMemory(memory)
{
    USAGI_ASSERT_THROW(
        memory.size() > sizeof(Segment),
        std::runtime_error("Insufficient space.")
    );
    init();
}

void CircularAllocator::head_increment(const std::size_t offset)
{
    mHead += offset;
    mHead %= mMemory.size();
}

void CircularAllocator::set_tail(const std::size_t offset)
{
    mTail = offset;
    mTail %= mMemory.size();
}

void * CircularAllocator::allocate(const std::size_t size)
{
    USAGI_ASSERT_THROW(size, std::bad_alloc());

    // Case 1: ... head ... tail ...
    // [A. check the free space after tail.] because the tail will wrap back
    // when it reaches the end, there is always free space at the tail if
    // tail > head. if the space is enough, allocate there. otherwise, [B. check
    // if the space before head is large enough.] if so, allocate there.
    // otherwise fail the allocation.
    //
    // Case 2: ... tail ... head ...
    // [A. check is there free space between the tail and the head. if so,
    // allocate there.] [C. otherwise fail the allocation.]
    //
    // Case 3: head == tail
    // the memory is either fully freed or fully allocated. if it's fully free,
    // it is already handled by [A] so [C. if that failed, there wasn't enough
    // space and the allocation should fail.] if the memory is fully allocated,
    // [A] also fails it because there wasn't enough free space on the tail.
    // so this case does not need special handling.

    // A. first try to allocate from the tail free segment. the end of that
    // segment may be the end of managed memory or head. if head == tail and
    // the buffer is full, this automatically fails because the tail segment
    // overlapping with the head is marked used.
    Segment *tail = as_segment(mTail);
    if(!tail->can_allocate(size))
    {
        // if tail < head, the allocation fails because the previous attempt
        // shows no space left. if head == tail and there was enough space, the
        // tail allocation should have succeeded. so it's only possible to
        // continue with head < tail. if there is no space before head, also
        // fail.
        if(mTail <= mHead) USAGI_THROW(std::bad_alloc());
        // B.
        tail = as_segment(0);
        // if there is space before head, there must be a free segment.
        assert(tail->freed);
        // there should only be at most one free segment before the head.
        assert(tail->length == mHead);
        if(!tail->can_allocate(size)) USAGI_THROW(std::bad_alloc());
    }
    // calculate the free space left in the tail segment after allocation.
    const std::size_t leftover = tail->do_allocate(size);
    set_tail(offset_from_segment(tail) + tail->length);
    // if there is space left, create a free segment there to maintain
    // the linked list.
    if(leftover) init_free_segment(mTail, leftover);
    return tail->payload();
}

void CircularAllocator::deallocate(void *ptr)
{
    USAGI_ASSERT_THROW(allocated_by_us(ptr), std::bad_alloc());

    // ... tail ... head ... begin ...
    // 
    // scan to merge subsequent free segments
    // 
    // merge free segments starting from begin, stop if [A. the end of memory]
    // or [B. the tail], or [C. an allocated segment] is met.
    // if begin == head, increment head.
    // if the stop was due to A., wrap back to the beginning of the memory,
    // merge free segments until tail is met.
    // if the stop was due to C., return,

    SegmentIterator it(this, segment_from_payload(ptr));
    assert(it != end());
    // free the requested segment.
    assert(it->freed == false);
    it->freed = true;

    // loop to merge subsequent free segments.
    while(true)
    {
        SegmentIterator next = it.linked_next();
        // [B.]/[C.] we are done merging the segments.
        if(!next->freed || next == tail())
        {
            // we can't merge the next segment. if the current segment is
            // not head, leave it alone. it might be merged from ahead later.
            // otherwise, remove this segment by advancing the head.
            // fragmentation is possible because we can't merge with segments
            // before the current one. but when the head segment is freed,
            // this segment will be eventually merged.
            it.commit_possible_head_advance();
            break;
        }
        // if the next wraps back to the beginning, we are done with the
        // back part of the segments. commit the changes and merge from the
        // beginning. the case where the beginning is the tail is handled
        // above.
        if(next == begin())
        {
            it.commit_possible_head_advance();
            it = begin();
            continue;
        }
        assert(next->freed);
        // merge next segment. 
        it->length += next->length;
    }

    // because we are releasing the segments, if the head meets the tail it
    // must be the case that all the segments are freed. reset the allocator
    // then.
    if(head() == tail()) init();
}

bool CircularAllocator::check_integrity()
{
    std::size_t total_bytes = 0;
    for(auto it = begin(); it != end(); it = it.seq_next())
        total_bytes += it->length;
    return total_bytes == mMemory.size();
}

void CircularAllocator::print(std::ostream &os)
{
    os << "[Segments]\n";
    for(auto it = begin(); it != end(); it = it.seq_next())
    {
        if(it->freed) os << "free ";
        else os << "used ";
        os << it->length << " bytes ";
        if(it == head()) os << "(head) ";
        if(it == tail()) os << "(tail) ";
        os << "\n";
    }
}
}
