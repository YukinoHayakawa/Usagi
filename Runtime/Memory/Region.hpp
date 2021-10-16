#pragma once

#include <cstddef>

namespace usagi
{
struct ReadonlyMemoryRegion
{
    const void *base_address = nullptr;
    std::size_t length = 0;
};

struct MemoryRegion
{
    void *base_address = nullptr;
    std::size_t length = 0;

    operator ReadonlyMemoryRegion() const
    {
        return { base_address, length };
    }
};

struct OffsetRegion
{
    std::size_t offset = 0;
    std::size_t length = 0;
};
}
