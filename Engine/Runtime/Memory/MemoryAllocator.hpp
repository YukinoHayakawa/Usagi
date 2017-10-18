#pragma once

namespace yuki
{

class MemoryAllocator
{
public:
    virtual ~MemoryAllocator() = default;

    virtual size_t allocate(size_t size, size_t alignment) = 0;
    virtual void release(size_t offset) = 0;
    virtual void reset() = 0;
};

}
