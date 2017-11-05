#pragma once

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki::graphics
{

// todo ring buffer across frames
class MemoryAllocator : Noncopyable
{
public:
    virtual ~MemoryAllocator() = default;

    struct Allocation
    {
        size_t pool_id, size;
        size_t offset;
    };
    virtual Allocation allocate(size_t size) = 0;
    virtual void release(const Allocation &alloc_info) = 0;
    virtual Allocation reallocate(const Allocation &alloc_info) = 0;
    virtual void reset() = 0;

    virtual void * getMappedAddress(const Allocation &allocation) const = 0;
    /**
     * \brief 
     * \param allocation 
     * \param offset Offset relative to the allocation,
     * \param size 
     */
    virtual void flushRange(const Allocation &allocation, size_t offset, size_t size) = 0;

    /**
     * \brief An allocator may delegate the actual allocation tasks to another.
     * For example, the FrameController multiplexes the allocation to different
     * allocators by the index of current frame. Since Buffer implementation
     * may cooperate closely with the underlying allocator, a way is required to
     * obtain the actual implementation of the allocator.
     * todo: potential flaw: when the buffer want to access the actual allocator, the frame is already swapped thus result in incorrect returning value.
     * \return 
     */
    virtual MemoryAllocator * getActualAllocator(const Allocation &allocation) = 0;
};

}
