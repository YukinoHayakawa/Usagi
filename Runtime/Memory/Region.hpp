#pragma once

#include <cstddef>
#include <string_view>

namespace usagi
{
struct ReadonlyMemoryRegion
{
    const void *base_address = nullptr;
    std::size_t length = 0;

    operator bool() const
    {
        return base_address && length;
    }

    static ReadonlyMemoryRegion from_string_view(std::string_view str)
    {
        return { str.data(), str.size() };
    }

    std::string_view to_string_view() const
    {
        return { static_cast<const char *>(base_address), length };
    }

    const char * as_chars() const
    {
        return static_cast<const char *>(base_address);
    }
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
