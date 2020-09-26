#pragma once

#include <cassert>
#include <type_traits>

namespace usagi
{
// https://stackoverflow.com/questions/3407012/c-rounding-up-to-the-nearest-multiple-of-a-number
template <typename T>
T round_up_unsigned(T num_to_round, T multiple)
    requires std::is_unsigned_v<T>
{
    assert(multiple > 0);

    const T remainder = num_to_round % multiple;
    if(remainder == 0)
        return num_to_round;

    return num_to_round + multiple - remainder;
}

template <typename T>
T round_down_unsigned(T num_to_round, T multiple)
    requires std::is_unsigned_v<T>
{
    assert(multiple > 0);

    const T remainder = num_to_round % multiple;

    return num_to_round - remainder;
}

template <typename T>
T calculate_spanning_pages(T length, T page_length)
    requires std::is_unsigned_v<T>
{
    assert(page_length >= 1);

    T pages = length / page_length;
    if(length % page_length != 0)
        ++pages;

    return pages;
}
}
