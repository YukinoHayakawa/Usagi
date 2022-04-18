#pragma once

#include <Usagi/Library/Memory/Arithmetic.hpp>
#include <Usagi/Library/Memory/Noncopyable.hpp>
#include <Usagi/Runtime/Memory/View.hpp>

namespace usagi
{
/**
 * \brief Circular allocator that supports ahead-of-time release of blocks.
 *
 * invariants:
 *  1. memory.length > sizeof(Segment) (so invariant 2. can be maintained).
 *  2. there is always a segment at the beginning of the managed memory.
 *  3. if head or tail reaches the end, it get wrapped back to 0.
 *  4. if head == tail, the memory is either fully allocated or fully freed.
 *  5. if the memory is fully freed, head == tail == 0.
 *
 * todo: impl alignment
 * todo: impl move ops
 */
class CircularAllocator : Noncopyable
{
protected:
    MemoryView mMemory;

    struct Segment
    {
        std::size_t freed : 1 = 0;
        // length of current segment. the size of allocatable memory is 8 bytes
        // less (due to overhead of this header).
        std::size_t length : 63 = 0;

        void * payload();

        bool can_allocate(std::size_t size) const;
        // return the free space of next segment
        std::size_t do_allocate(std::size_t size);
    };

    class SegmentIterator
    {
        CircularAllocator *mAllocator = nullptr;
        Segment *mSegment = nullptr;

    public:
        SegmentIterator() = default;
        SegmentIterator(CircularAllocator *allocator, Segment *segment);

        // next segment in the sequential order in the memory.
        SegmentIterator seq_next() const;
        // next segment in the linked list.
        SegmentIterator linked_next() const;

        auto operator->() const { return mSegment; }
        bool operator==(const SegmentIterator &) const = default;
        void commit_possible_head_advance() const;
        bool can_allocate(std::size_t size) const;
    };

    friend class SegmentIterator;

    std::size_t mHead = 0;
    std::size_t mTail = 0;

    // ReSharper disable once CppMemberFunctionMayBeConst
    Segment * as_segment(const std::size_t offset)
    {
        return raw_advance(mMemory.as_mutable<Segment>(), offset);
    }

    Segment * from_payload(void *ptr);

    SegmentIterator head(); // head segment
    SegmentIterator tail(); // tail segment
    SegmentIterator begin(); // first segment (sequentially in the memory)
    SegmentIterator end(); // last segment
    SegmentIterator at(std::size_t offset);

    void head_increment(std::size_t offset);
    void tail_increment(std::size_t offset);

    bool allocated_by_us(void *ptr) const;

    void init();

    Segment * init_free_segment(std::size_t offset, std::size_t length);

public:
    CircularAllocator() = default;
    explicit CircularAllocator(MemoryView memory);

    void * allocate(std::size_t size);
    void deallocate(void *ptr);

    // validate that the segments can be traversed as a linked list that
    // covers all the managed memory.
    bool check_integrity();
    void print(std::ostream &os);
};
}
