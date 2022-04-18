#pragma once

#include <Usagi/Library/Memory/Arithmetic.hpp>
#include <Usagi/Library/Memory/Noncopyable.hpp>
#include <Usagi/Runtime/Memory/View.hpp>

namespace usagi
{
/**
 * \brief Circular allocator that supports ahead-of-time release of blocks.
 * todo: impl alignment
 * todo: impl move ops
 */
class CircularAllocator : Noncopyable
{
protected:
    MemoryView mMemory;

    // todo impl block list
    struct Segment
    {
        std::size_t freed : 1 = 0;
        // length of current segment. the size of allocated memory is 8 bytes
        // less (due to this header).
        std::size_t length : 63 = 0;

        static std::size_t make_segment_size(const std::size_t alloc_size)
        {
            return alloc_size + sizeof(Segment);
        }

        void * payload()
        {
            return raw_advance(this, sizeof(Segment));
        }
    };

    class SegmentIterator
    {
        CircularAllocator *mAllocator;
        Segment *mSegment;

    public:
        SegmentIterator() = default;
        SegmentIterator(CircularAllocator *allocator, Segment *segment);

        auto operator->() const { return mSegment; }

        SegmentIterator next() const
        {
            return SegmentIterator(
                mAllocator,
                raw_advance(mSegment, mSegment->length)
            );
        }

        std::partial_ordering operator<=>(const SegmentIterator &rhs) const
        {
            if(mAllocator != rhs.mAllocator)
                return std::partial_ordering::unordered;
            return mSegment <=> rhs.mSegment;
        }

        bool operator==(const SegmentIterator &) const = default;

        bool try_merge_next();
        bool can_allocate(std::size_t size) const;
    };

    friend class SegmentIterator;

    Segment *mHead = nullptr;
    Segment *mTail = nullptr;

    // ReSharper disable once CppMemberFunctionMayBeConst
    Segment * as_segment(const std::size_t offset)
    {
        return raw_advance(mMemory.as_mutable<Segment>(), offset);
    }

    Segment * from_payload(void *ptr);

    SegmentIterator head();
    SegmentIterator end();

    bool can_tail_allocate(size_t size) const;
    void * commit_tail_allocate(size_t size);
    bool can_head_allocate(size_t size);
    void wrap_back();
    bool allocated_by_us(void *ptr) const;
    void free_segment(void *ptr);
    void merge_head_segments();

    void init_segment();

public:
    CircularAllocator() = default;
    explicit CircularAllocator(MemoryView memory);

    void * allocate(std::size_t size);
    void deallocate(void *ptr);
};
}
