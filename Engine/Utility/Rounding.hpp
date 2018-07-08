#pragma once

#include <cassert>

namespace usagi::utility
{

// https://stackoverflow.com/questions/3407012/c-rounding-up-to-the-nearest-multiple-of-a-number
template <typename T>
T roundUpUnsigned(T num_to_round, T multiple)
{
    assert(num_to_round >= 0);
    assert(multiple > 0);

    const T remainder = num_to_round % multiple;
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
