#pragma once

#include <memory>

namespace yuki
{

class GraphicsResourceAllocator
{
public:
    virtual ~GraphicsResourceAllocator() = default;

    // dynamic buffers are allocated from a stack allocator which reset at
    // the beginning of each frame. the user must reallocate and fill in
    // the data when preparing for each frame.
    virtual std::unique_ptr<class GraphicsBuffer> createDynamicBuffer(size_t size) = 0;
};

}
