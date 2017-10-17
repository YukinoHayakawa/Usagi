#pragma once

#include <Usagi/Engine/Runtime/Exception.hpp>
#include "MemoryAllocator.hpp"
#include <Usagi/Engine/Utility/Rounding.hpp>

namespace yuki
{

class StackMemoryAllocator : public MemoryAllocator
{
    const size_t mMemoryBegin;
    size_t mCurrentPos;
    const size_t mTotalSize;

public:
    StackMemoryAllocator(size_t memory_begin, size_t total_size);

    void reset() override;
    size_t allocate(size_t size, size_t alignment) override;
    void release(size_t offset) override;
};

}
