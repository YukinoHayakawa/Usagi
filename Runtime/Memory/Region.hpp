#pragma once

#include <cstddef>

namespace usagi
{
struct MemoryRegion
{
    void *base_address = nullptr;
    std::size_t length = 0;
};

struct OffsetRegion
{
    std::size_t offset = 0;
    std::size_t length = 0;
};
}
