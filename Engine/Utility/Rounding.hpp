#pragma once

#include <type_traits>
#include <cassert>

namespace yuki::utility
{

// https://stackoverflow.com/questions/3407012/c-rounding-up-to-the-nearest-multiple-of-a-number
template <typename UInt>
UInt roundUpUnsigned(UInt num_to_round, UInt multiple)
{
    static_assert(std::is_unsigned_v<UInt>, "UInt is not unsigned type.");

    if(multiple == 0)
        return num_to_round;

    const UInt remainder = num_to_round % multiple;
    if(remainder == 0)
        return num_to_round;

    return num_to_round + multiple - remainder;
}

template <typename T>
T calculateSpanningPages(T length, T page_length)
{
    assert(length >= 0);
    assert(page_length >= 1);

    T pages = length / page_length;
    if(length % page_length != 0)
        ++pages;

    return pages;
}

}
