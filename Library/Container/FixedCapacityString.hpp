#pragma once

#include <cstddef>
#include <string_view>

namespace usagi
{
template <std::size_t Size>
class FixedCapacityString
{
    char mString[Size] = { };

public:
    std::string_view str() const
    {
        if(mString[Size - 1] != 0)
            return { mString, Size };
        return { mString };
    }

    auto & operator=(const std::string_view rhs)
    {
        const auto copy_size = std::min(rhs.size(), Size);
        std::memcpy(mString, rhs.data(), copy_size);
        if(copy_size < Size)
            mString[copy_size] = 0;
        return *this;
    }

    bool operator==(const std::string_view rhs) const
    {
        return str() == rhs;
    }
};
}
