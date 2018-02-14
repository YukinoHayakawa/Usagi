#pragma once

namespace yuki::utility
{
// https://stackoverflow.com/questions/108318/whats-the-simplest-way-to-test-whether-a-number-is-a-power-of-2-in-c
template <typename T>
bool isPowerOfTwoOrZero(T value)
{
    return (value & (value - 1)) == 0;
}

template <typename T>
bool isPowerOfTwo(T value)
{
    return (value & (value - 1)) == 0 && value != 0;
}
}
