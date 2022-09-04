#pragma once

#include <cstddef>

namespace usagi
{
template <typename T, typename Distance>
T * raw_advance(T *base, Distance offset)
{
    union
    {
        T *address { };
        char *bytes;
    };

    address = static_cast<T *>(base);
    bytes += offset;

    return address;
}

template <typename T, typename Distance>
T * raw_advance_cast(void *base, Distance offset)
{
    return raw_advance(
        static_cast<T *>(base),
        offset
    );
}

template <typename T0, typename T1>
std::ptrdiff_t address_diff(const T0 *base, const T1 *another)
{
    const auto b = reinterpret_cast<const char *>(base);
    const auto a = reinterpret_cast<const char *>(another);

    return a - b;
}
}
