#pragma once

#include <memory>

#include <Usagi/Engine/Utility/Noncopyable.hpp>

namespace yuki::graphics
{

class ResourceAllocator : Noncopyable
{
public:
    virtual ~ResourceAllocator() = default;

    // dynamic buffers are allocated from a stack allocator which reset at
    // the beginning of each frame. the user must reallocate and fill in
    // the data when preparing for each frame.
    virtual std::unique_ptr<class Buffer> createDynamicBuffer(size_t size) = 0;
};

}
