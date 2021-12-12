#pragma once

#include <cassert>
#include <type_traits>

namespace usagi
{
// https://stackoverflow.com/questions/3407012/c-rounding-up-to-the-nearest-multiple-of-a-number
template <typename T>
T align_up(T address, T alignment)
    requires std::is_unsigned_v<T>
{
    assert(alignment > 0);

    const T remainder = address % alignment;
    if(remainder == 0)
        return address;

    return address + alignment - remainder;
}

template <typename T>
T align_down(T address, T alignment)
    requires std::is_unsigned_v<T>
{
    assert(alignment > 0);

    const T remainder = address % alignment;

    return address - remainder;
}

template <typename T>
T calculate_spanning_pages(T size, T page_size)
    requires std::is_unsigned_v<T>
{
    assert(page_size >= 1);

    T pages = size / page_size;
    if(size % page_size != 0)
        ++pages;

    return pages;
}
}
